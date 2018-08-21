defmodule WebengineKiosk do
  use GenServer
  alias WebengineKiosk.{Message, Options}

  require Logger

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
  Control a fullscreen web browser using Elixir for use in a kiosk.



  """

  @doc """
  Start the kiosk.

  The kiosk starts fullscreen and goes to a default local web page. To change
  this, set one or more options:

  * `background_color: color` - specify a background color as #RRGGBB or by name
  * `blank_image: path` - specify a path to an image for when the screen is blanked
  * `fullscreen: boolean` - show fullscreen
  * `gid: gid` - run the browser with this group id
  * `homepage: url` - load this page first. For local files, specify `file:///path/to/index.html`
  * `opengl: "gl" | "gles" | "software" | "auto"` - specify the OpenGL backend. This is only a hint.
  * `progress: boolean` - show a progress bar when loading pages
  * `sounds: boolean` - play sounds on clicks
  * `uid: uid` - run the browser as this user

  Untested:

  * `monitor: index` - select the monitor for the webbrowser (0, 1, etc.)
  * `clear_cache: boolean`
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
  """
  @spec start_link(Keyword.t(), GenServer.options()) :: {:ok, pid} | {:error, term}
  def start_link(args, genserver_opts \\ []) do
    with :ok <- Options.check_args(args) do
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

  The web browser will be replaced by a screen with the `blank_image`. If
  someone clicks or taps on the screen then a wakeup message will be sent.
  While the screen is in the blank state, it can still accept requests to go to
  other URLs.
  """
  @spec blank(GenServer.server()) :: :ok
  def blank(server) do
    GenServer.call(server, {:blank, true})
  end

  @doc """
  Unblank the screen

  Show the web browser again.
  """
  @spec unblank(GenServer.server()) :: :ok
  def unblank(server) do
    GenServer.call(server, {:blank, false})
  end

  @doc """
  Request that the browser go to the homepage.
  """
  @spec go_home(GenServer.server()) :: :ok | {:error, term}
  def go_home(server) do
    GenServer.call(server, :go_home)
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
    priv_dir = :code.priv_dir(:webengine_kiosk)
    cmd = Path.join(priv_dir, "kiosk")

    all_options = Options.add_defaults(args)

    cmd_args =
      all_options
      |> Enum.flat_map(fn {key, value} -> ["--#{key}", to_string(value)] end)

    homepage = Keyword.get(all_options, :homepage)

    port =
      Port.open({:spawn_executable, cmd}, [
        {:args, cmd_args},
        {:cd, priv_dir},
        {:packet, 2},
        :use_stdio,
        :binary,
        :exit_status
      ])

    {:ok, %{port: port, homepage: homepage}}
  end

  def handle_call(:go_home, _from, state) do
    send_port(state, Message.go_to_url(state.homepage))
    {:reply, :ok, state}
  end

  def handle_call({:go_to_url, url}, _from, state) do
    send_port(state, Message.go_to_url(url))
    {:reply, :ok, state}
  end

  def handle_call({:run_javascript, code}, _from, state) do
    send_port(state, Message.run_javascript(code))
    {:reply, :ok, state}
  end

  def handle_call({:blank, yes}, _from, state) do
    send_port(state, Message.blank(yes))
    {:reply, :ok, state}
  end

  def handle_info({_, {:data, raw_message}}, state) do
    msg = Message.decode(raw_message)
    Logger.debug("webengine_kiosk: received #{inspect(msg)}")
    {:noreply, state}
  end

  def handle_info({port, {:exit_status, 0}}, %{port: port} = state) do
    Logger.info("webengine_kiosk: normal exit from port")
    {:stop, :normal, state}
  end

  def handle_info({port, {:exit_status, status}}, %{port: port} = state) do
    Logger.error("webengine_kiosk: unexpected exit from port: #{status}")
    {:stop, :unexpected_exit, state}
  end

  defp send_port(state, message) do
    send(state.port, {self(), {:command, message}})
  end
end
