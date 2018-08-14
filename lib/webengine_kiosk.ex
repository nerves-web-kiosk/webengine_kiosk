defmodule WebengineKiosk do
  use GenServer
  require Logger

  @msg_go_to_url 1
  @msg_run_javascript 2
  @msg_loading_page 3
  @msg_progress 4
  @msg_finished_loading_page 5
  @msg_url_changed 6
  @msg_blank 7

  def child_spec({opts, genserver_opts}) do
    id = genserver_opts[:id] || __MODULE__
    %{
      id: id,
      start: {__MODULE__, :start_link, [opts, genserver_opts]}
    }
  end
  def child_spec(opts) do
    child_spec({opts, []})
  end

  @moduledoc """
  Start up and control a browser.

  """

  @doc """
  Start the kiosk.

  The kiosk starts fullscreen and goes to a default local web page. To change this,
  set one or more options:

  * `background_color: color` - specify a background color as #RRGGBB or by name
  * `blank_image: path` - specify a path to an image for when the screen is blanked
  * `fullscreen: boolean` - show fullscreen
  * `homepage: url` - load this page first. For local files, specify `file:///path/to/index.html`
  * `progress: boolean` - show a progress bar when loading pages
  * `sounds: boolean` - play sounds on clicks

  Untested:

  * `monitor: index` - select the monitor for the webbrowser (0, 1, etc.)
  * `clear_cache: boolean`
  * `opengl: "gl" | "gles" | "software" | "auto"` - specify the OpenGL backend to use
  * `proxy_enable: boolean` - enable/disable proxy support
  * `proxy_system: ` -
  * `proxy_host: hostname` - the host to connect to for using a proxy
  * `proxy_port: port` - the port to connect to on the proxy
  * `proxy_username: username` - a username for the proxy
  * `proxy_password: password` - a password for the proxy
  * `stay_on_top: boolean` -
  * `window_clicked_sound: url` - a sound to play when the window is clicked
  * `link_clicked_sound: url` - a sound to play when a link is clicked
  * `hide_cursor: boolean` - show or hide the mouse pointer
  * `javascript: boolean` - enable or disable Javascript support
  * `javascript_can_open_windows: boolean` - allow Javascript to open windows
  * `debug_menu: boolean` - enable a menubar for debugging
  * `width: pixels` - when not fullscreen, the window is this width
  * `height: pixels` - when not fullscreen, the window is this height
  * `uid: uid` - run the browser as this user
  * `gid: gid` - run the browser with this group id
  """
  @spec start_link(Keyword.t(), GenServer.options()) :: {:ok, pid} | {:error, term}
  def start_link(args, genserver_opts \\ []) do
    with :ok <- check_args(args) do
      GenServer.start_link(__MODULE__, args, genserver_opts)
    end
  end

  @doc """
  Stop the kiosk
  """
  @spec stop(GenServer.server()) :: :ok
  def stop(server) do
    GenServer.stop(server)
  end

  @doc """
  Blank the screen
  """
  @spec blank(GenServer.server(), boolean()) :: :ok
  def blank(server, yes) do
    GenServer.call(server, {:blank, yes})
  end

  @doc """
  Request that the browser go to the specified URL.
  """
  @spec go_to_url(GenServer.server(), String.t()) :: :ok | {:error, term}
  def go_to_url(server, url) do
    GenServer.call(server, {:go_to_url, url})
  end

  @doc """
  Run arbitrary Javascript in the browser
  """
  @spec run_javascript(GenServer.server(), String.t()) :: :ok | {:error, term}
  def run_javascript(server, code) do
    GenServer.call(server, {:run_javascript, code})
  end

  def init(args) do
    cmd = Path.join(:code.priv_dir(:webengine_kiosk), "kiosk")
    cmd_args = Enum.flat_map(args, &to_cmd_option/1)

    port =
      Port.open({:spawn_executable, cmd}, [
        {:args, cmd_args},
        {:packet, 2},
        :use_stdio,
        :binary,
        :exit_status
      ])

    {:ok, port}
  end

  def handle_call({:go_to_url, url}, _from, state) do
    msg = <<@msg_go_to_url, url::binary>>
    send(state, {self(), {:command, msg}})
    {:reply, :ok, state}
  end

  def handle_call({:run_javascript, code}, _from, state) do
    msg = <<@msg_run_javascript, code::binary>>
    send(state, {self(), {:command, msg}})
    {:reply, :ok, state}
  end

  def handle_call({:blank, yes}, _from, state) do
    msg = blank_message(yes)
    send(state, {self(), {:command, msg}})
    {:reply, :ok, state}
  end

  defp blank_message(true), do: <<@msg_blank, 1>>
  defp blank_message(false), do: <<@msg_blank, 0>>

  def handle_info({_, {:data, msg}}, state) do
    dispatch_message(msg, state)
    {:noreply, state}
  end

  def handle_info({state, {:exit_status, status}}, state) do
    Logger.error("The kiosk exited with status #{status}")
    {:stop, :normal, state}
  end

  defp dispatch_message(<<@msg_progress, value>>, _state) do
    Logger.debug("Progress #{value}")
  end

  defp dispatch_message(<<@msg_url_changed, url::binary>>, _state) do
    Logger.debug("URL changed to #{url}")
  end

  defp dispatch_message(<<@msg_loading_page>>, _state) do
    Logger.debug("Starting to load page")
  end

  defp dispatch_message(<<@msg_finished_loading_page>>, _state) do
    Logger.debug("Finished loading page")
  end

  defp dispatch_message(<<msg_type, _payload::binary>>, _state) do
    Logger.error("Received unknown message from kiosk port. Type=#{msg_type}")
  end

  defp to_cmd_option({key, value}) do
    ["--#{key}", to_string(value)]
  end

  @arguments [
    :clear_cache,
    :homepage,
    :monitor,
    :opengl,
    :proxy_enable,
    :proxy_system,
    :proxy_host,
    :proxy_port,
    :proxy_username,
    :proxy_password,
    :stay_on_top,
    :progress,
    :sounds,
    :window_clicked_sound,
    :link_clicked_sound,
    :hide_cursor,
    :javascript,
    :javascript_can_open_windows,
    :debug_menu,
    :fullscreen,
    :width,
    :height,
    :uid,
    :gid,
    :blank_image,
    :background_color
  ]

  defp check_args(args) do
    case Enum.find(args, &invalid_arg?/1) do
      nil -> :ok
      arg -> {:error, "Unknown option #{inspect(arg)}"}
    end
  end

  defp invalid_arg?({arg, _value}) do
    !(arg in @arguments)
  end
end
