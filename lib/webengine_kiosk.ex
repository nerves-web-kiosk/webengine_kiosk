defmodule WebengineKiosk do
  use GenServer
  require Logger

  @msg_go_to_url 1
  @msg_run_javascript 2
  @msg_loading_page 3
  @msg_progress 4
  @msg_finished_loading_page 5
  @msg_url_changed 6


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
  Documentation for WebengineKiosk.
  """

  @doc """
  Start a kiosk

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
  def stop(pid) do
    GenServer.stop(pid)
  end

  @doc """
  Request that the browser go to the specified URL.
  """
  @spec go_to_url(GenServer.server(), String.t()) :: :ok | {:error, term}
  def go_to_url(pid, url) do
    GenServer.call(pid, {:go_to_url, url})
  end

  @doc """
  Run arbitrary Javascript in the browser
  """
  @spec run_javascript(GenServer.server(), String.t()) :: :ok | {:error, term}
  def run_javascript(pid, code) do
    GenServer.call(pid, {:run_javascript, code})
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
    :gid
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
