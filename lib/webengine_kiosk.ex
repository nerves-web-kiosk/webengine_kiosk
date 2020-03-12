defmodule WebengineKiosk do
  use Supervisor

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
  Control a fullscreen web browser using Elixir for use in a kiosk
  """

  @doc """
  Start the kiosk.

  The kiosk starts fullscreen and goes to a default local web page. To change
  this, set one or more options:

  * `background_color: color` - specify a background color as #RRGGBB or by name
  * `blank_image: path` - specify a path to an image for when the screen is blanked
  * `data_dir: path` - specify a writable path for data files
  * `debug_keys: boolean` - enable key combinations useful for debugging
  * `fullscreen: boolean` - show fullscreen
  * `gid: gid` - run the browser with this group id
  * `homepage: url` - load this page first. For local files, specify `file:///path/to/index.html`
  * `monitor: index` - select the monitor for the web browser (0, 1, etc.)
  * `opengl: "gl" | "gles" | "software" | "auto"` - specify the OpenGL backend. This is only a hint.
  * `progress: boolean` - show a progress bar when loading pages
  * `run_as_root: boolean` - set to true if you really want to run Chromium as root
  * `sounds: boolean` - play sounds on clicks
  * `uid: uid` - run the browser as this user

  Untested:

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
  * `width: pixels` - when not fullscreen, the window is this width
  * `height: pixels` - when not fullscreen, the window is this height
  """
  @spec start_link(Keyword.t(), GenServer.options()) :: {:ok, pid} | {:error, term}
  def start_link(args, genserver_opts \\ []) do
    Supervisor.start_link(__MODULE__, args, genserver_opts)
  end

  @doc """
  Stop the kiosk
  """
  @spec stop(Supervisor.supervisor()) :: :ok
  def stop(server) do
    Supervisor.stop(server)
  end

  @impl true
  def init(init_arg) do
    registry_name =
      "webengine_kiosk_registry_#{System.unique_integer([:positive])}" |> String.to_atom()

    children = [
      Supervisor.child_spec({Registry, keys: :duplicate, name: registry_name}, id: :registry),
      Supervisor.child_spec({WebengineKiosk.Kiosk, %{args: init_arg, parent: self()}}, id: :kiosk)
    ]

    Supervisor.init(children, strategy: :one_for_one)
  end

  @doc """
  Blank the screen

  The web browser will be replaced by a screen with the `blank_image`. If
  someone clicks or taps on the screen then a wakeup message will be sent.
  While the screen is in the blank state, it can still accept requests to go to
  other URLs.
  """
  @spec blank(Supervisor.supervisor()) :: :ok
  def blank(server), do: call_kiosk(server, {:blank, true})

  @doc """
  Unblank the screen

  Show the web browser again.
  """
  @spec unblank(Supervisor.supervisor()) :: :ok
  def unblank(server), do: call_kiosk(server, {:blank, false})

  @doc """
  Request that the browser go to the homepage.
  """
  @spec go_home(Supervisor.supervisor()) :: :ok | {:error, term}
  def go_home(server), do: call_kiosk(server, :go_home)

  @doc """
  Request that the browser go to the specified URL.
  """
  @spec go_to_url(Supervisor.supervisor(), String.t()) :: :ok | {:error, term}
  def go_to_url(server, url), do: call_kiosk(server, {:go_to_url, url})

  @doc """
  Run Javascript in the browser.
  """
  @spec run_javascript(Supervisor.supervisor(), String.t()) :: :ok | {:error, term}
  def run_javascript(server, code), do: call_kiosk(server, {:run_javascript, code})

  @doc """
  Reload the current page.
  """
  @spec reload(Supervisor.supervisor()) :: :ok | {:error, term()}
  def reload(server), do: call_kiosk(server, :reload)

  @doc """
  Go to the previously visited page.
  """
  @spec back(Supervisor.supervisor()) :: :ok | {:error, term()}
  def back(server), do: call_kiosk(server, :back)

  @doc """
  Go forward in history.
  """
  @spec forward(Supervisor.supervisor()) :: :ok | {:error, term()}
  def forward(server), do: call_kiosk(server, :forward)

  @doc """
  Stop loading the current page.
  """
  @spec stop_loading(Supervisor.supervisor()) :: :ok | {:error, term()}
  def stop_loading(server), do: call_kiosk(server, :stop_loading)

  @doc """
  Set the zoom factor for displaying the page.
  """
  @spec set_zoom(Supervisor.supervisor(), number()) :: :ok | {:error, term()}
  def set_zoom(server, factor) when is_number(factor) and factor > 0 do
    call_kiosk(server, {:set_zoom, factor})
  end

  @doc """
  Register calling process to receive events
  """
  @spec register(Supervisor.supervisor()) :: {:ok, pid} | {:error, term()}
  def register(server) do
    Registry.register(registry_name(server), "events", [])
  end

  @doc """
  Unregister calling process from receiving events
  """
  @spec unregister(Supervisor.supervisor()) :: :ok
  def unregister(server) do
    Registry.unregister(registry_name(server), "events")
  end

  @spec dispatch_event(Supervisor.supervisor(), any) :: :ok
  def dispatch_event(server, event) do
    Registry.dispatch(registry_name(server), "events", fn entries ->
      for {pid, _} <- entries, do: send(pid, event)
    end)
  end

  @spec registry_name(Supervisor.supervisor()) :: atom()
  defp registry_name(server) do
    {:ok, pid} = find_child(server, :registry)
    {:registered_name, reg_name} = Process.info(pid, :registered_name)
    reg_name
  end

  @spec call_kiosk(Supervisor.supervisor(), term) :: term
  defp call_kiosk(server, msg) do
    {:ok, pid} = find_child(server, :kiosk)
    GenServer.call(pid, msg)
  end

  @spec find_child(Supervisor.supervisor(), atom()) :: {:ok, pid} | :error
  defp find_child(supervisor, id) do
    ret =
      Supervisor.which_children(supervisor)
      |> Enum.find(fn {ch_id, _, _, _} -> ch_id == id end)

    if ret != nil do
      {_, pid, _, _} = ret
      {:ok, pid}
    else
      :error
    end
  end
end
