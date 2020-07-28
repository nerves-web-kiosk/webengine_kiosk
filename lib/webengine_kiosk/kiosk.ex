defmodule WebengineKiosk.Kiosk do
  use GenServer
  alias WebengineKiosk.{Message, Options}

  require Logger

  @moduledoc false

  @spec start_link(%{args: Keyword.t(), parent: pid}, GenServer.options()) ::
          {:ok, pid} | {:error, term}
  def start_link(%{args: args, parent: parent}, genserver_opts \\ []) do
    with :ok <- Options.check_args(args) do
      GenServer.start_link(__MODULE__, %{args: args, parent: parent}, genserver_opts)
    end
  end

  def init(%{args: args, parent: parent}) do
    priv_dir = :code.priv_dir(:webengine_kiosk)
    cmd = Path.join(priv_dir, "kiosk")

    if !File.exists?(cmd) do
      _ = Logger.error("Kiosk port application is missing. It should be at #{cmd}.")
      raise "Kiosk port missing"
    end

    all_options = Options.add_defaults(args)

    cmd_args =
      all_options
      |> Enum.flat_map(fn {key, value} -> ["--#{key}", to_string(value)] end)

    set_permissions(all_options)
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

    {:ok, %{port: port, homepage: homepage, parent: parent}}
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

  def handle_call(:reload, _from, state) do
    send_port(state, Message.reload())
    {:reply, :ok, state}
  end

  def handle_call(:back, _from, state) do
    send_port(state, Message.go_back())
    {:reply, :ok, state}
  end

  def handle_call(:forward, _from, state) do
    send_port(state, Message.go_forward())
    {:reply, :ok, state}
  end

  def handle_call(:stop_loading, _from, state) do
    send_port(state, Message.stop_loading())
    {:reply, :ok, state}
  end

  def handle_call({:set_zoom, factor}, _from, state) do
    send_port(state, Message.set_zoom(factor))
    {:reply, :ok, state}
  end

  def handle_info({_, {:data, raw_message}}, state) do
    raw_message
    |> Message.decode()
    |> handle_browser_message(state)
  end

  def handle_info({port, {:exit_status, 0}}, %{port: port} = state) do
    _ = Logger.info("webengine_kiosk: normal exit from port")
    {:stop, :normal, state}
  end

  def handle_info({port, {:exit_status, status}}, %{port: port} = state) do
    _ = Logger.error("webengine_kiosk: unexpected exit from port: #{status}")
    {:stop, :unexpected_exit, state}
  end

  defp handle_browser_message({:browser_crashed, reason, _exit_status}, state) do
    _ =
      Logger.error(
        "webengine_kiosk: browser crashed: #{inspect(reason)}. Going home and hoping..."
      )

    send_event(state.parent, {:browser_crashed, reason})

    # Try to recover by going back home
    send_port(state, Message.go_to_url(state.homepage))
    {:noreply, state}
  end

  defp handle_browser_message({:console_log, log}, state) do
    _ = Logger.warn("webengine_kiosk(stderr): #{log}")
    send_event(state.parent, {:console_log, log})
    {:noreply, state}
  end

  defp handle_browser_message(message, state) do
    _ = Logger.debug("webengine_kiosk: received #{inspect(message)}")
    send_event(state.parent, message)
    {:noreply, state}
  end

  defp send_event(parent, event) do
    WebengineKiosk.dispatch_event(parent, event)
  end

  defp send_port(state, message) do
    send(state.port, {self(), {:command, message}})
  end

  defp set_permissions(opts) do
    # Check if we are on a raspberry pi
    if File.exists?("/dev/vchiq") do
      File.chgrp("/dev/vchiq", 28)
      File.chmod("/dev/vchiq", 0o660)
    end

    if data_dir = Keyword.get(opts, :data_dir) do
      File.mkdir_p(data_dir)

      if uid = Keyword.get(opts, :uid) do
        chown(data_dir, uid)
      end
    end
  end

  defp chown(file, uid) when is_binary(uid) do
    case System.cmd("id", ["-u", uid]) do
      {uid, 0} ->
        uid =
          String.trim(uid)
          |> String.to_integer()

        chown(file, uid)

      _ ->
        :error
    end
  end

  defp chown(file, uid) when is_integer(uid) do
    File.chown(file, uid)
  end
end
