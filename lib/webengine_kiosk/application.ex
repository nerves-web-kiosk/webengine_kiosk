defmodule WebengineKiosk.Application do
  # See https://hexdocs.pm/elixir/Application.html
  # for more information on OTP Applications
  @moduledoc false

  use Application

  def start(_type, _args) do
    # List all child processes to be supervised
    opts = Application.get_all_env(:webengine_kiosk)

    children =
      if Code.ensure_loaded?(SystemRegistry) do
        [
          {WebengineKiosk.InputEvent, opts}
        ]
      else
        []
      end

    # See https://hexdocs.pm/elixir/Supervisor.html
    # for other strategies and supported options
    opts = [strategy: :one_for_one, name: WebengineKiosk.Supervisor]
    Supervisor.start_link(children, opts)
  end
end
