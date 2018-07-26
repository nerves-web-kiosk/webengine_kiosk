defmodule WebengineKiosk do
  use GenServer

  @moduledoc """
  Documentation for WebengineKiosk.
  """

  @doc """
  Start a kiosk

  """
  @spec start_link(Keyword.t()) :: {:ok, pid} | {:error, term}
  def start_link(args) do
    with :ok <- check_args(args) do
    GenServer.start_link(__MODULE__, args)
    end
  end

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

  defp to_cmd_option({key, value}) do
    ["--#{key}", to_string(value)]
  end

  @arguments [:clear_cache, :homepage, :monitor, :opengl, :proxy_enable, :proxy_system, :proxy_host, :proxy_port, :proxy_username,
  :proxy_password, :stay_on_top, :progress, :event_sounds, :window_clicked_sound, :link_clicked_sound, :hide_cursor,
  :javascript, :javascript_can_open_windows, :debug_menu]

  defp check_args(args) do
    case Enum.find(args, &invalid_arg?/1) do
      nil -> :ok
      arg -> {:error, "Unknown option #{inspect arg}"}
    end
  end

  defp invalid_arg?({arg, _value}) do
    ! arg in @arguments
  end
end
