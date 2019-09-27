if Code.ensure_loaded?(SystemRegistry) do
  defmodule WebengineKiosk.InputEvent do
    use GenServer

    require Logger

    @dev "/dev"
    @group 1000

    def start_link(opts) do
      GenServer.start_link(__MODULE__, opts)
    end

    def init(opts) do
      Application.ensure_all_started(:system_registry)
      gid = opts[:gid] || @group
      SystemRegistry.register()

      {:ok,
       %{
         inputs: [],
         gid: gid(gid)
       }}
    end

    def handle_info(
          {:system_registry, :global, %{state: %{"subsystems" => %{"input" => inputs}}}},
          %{inputs: inputs} = state
        ) do
      {:noreply, state}
    end

    def handle_info(
          {:system_registry, :global, %{state: %{"subsystems" => %{"input" => inputs}}} = reg},
          state
        ) do
      inputs = Enum.reject(inputs, &(&1 in state.inputs))

      {events, other_inputs} =
        Enum.split_with(inputs, &String.starts_with?(List.last(&1), "event"))

      new_events =
        events
        |> Enum.map(&{&1, get_in(reg, &1)})
        |> Enum.reject(&is_nil(elem(&1, 1)))
        |> Enum.map(&{elem(&1, 0), Map.get(elem(&1, 1), "devname")})
        |> Enum.filter(&(chgrp(elem(&1, 1), state.gid) == :ok))
        |> Enum.map(&elem(&1, 0))

      inputs = new_events ++ other_inputs ++ state.inputs

      {:noreply, %{state | inputs: inputs}}
    end

    def handle_info({:system_registry, _, _}, state) do
      {:noreply, state}
    end

    defp chgrp(devname, gid) do
      path = Path.join(@dev, devname)
      _ = Logger.info("webengine_kiosk: chgrp #{path}")
      File.chgrp(path, gid)
    end

    defp gid(gid) when is_integer(gid), do: gid

    defp gid(gid) when is_binary(gid) do
      case File.read("/etc/group") do
        {:ok, groups} ->
          group_rec =
            groups
            |> String.split()
            |> Enum.find(&String.starts_with?(&1, gid))

          case group_rec do
            nil ->
              @group

            group_rec ->
              [_, _, gid | _] = String.split(group_rec, ":")
              String.to_integer(gid)
          end

        _ ->
          @group
      end
    end
  end
end
