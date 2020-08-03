defmodule WebengineKiosk.Options do
  @all_options [
    :clear_cache,
    :data_dir,
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
    :background_color,
    :run_as_root,
    :http_accept_language,
    :http_user_agent
  ]

  @moduledoc false

  @doc """
  Go through all of the arguments and check for bad ones.
  """
  def check_args(args) do
    case Enum.find(args, &invalid_arg?/1) do
      nil -> :ok
      arg -> {:error, "Unknown option #{inspect(arg)}"}
    end
  end

  defp invalid_arg?({key, _value}) do
    !(key in @all_options)
  end

  @doc """
  Add the default options to the user-supplied list.
  """
  def add_defaults(args) do
    Keyword.merge(defaults(), args)
  end

  defp defaults() do
    # This is a runtime function so that the system can be queried for some options
    homepage_file = Application.app_dir(:webengine_kiosk, "priv/www/index.html")

    [
      homepage: "file://" <> homepage_file,
      fullscreen: true,
      background_color: "black"
    ]
  end
end
