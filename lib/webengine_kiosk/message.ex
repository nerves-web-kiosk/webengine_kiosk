defmodule WebengineKiosk.Message do
  # These should match KioskMessage.h
  @msg_go_to_url 1
  @msg_run_javascript 2
  @msg_loading_page 3
  @msg_progress 4
  @msg_finished_loading_page 5
  @msg_url_changed 6
  @msg_blank 7
  @msg_wakeup 8
  @msg_reload 9
  @msg_go_back 10
  @msg_go_forward 11
  @msg_stop_loading 12
  @msg_set_zoom 13
  @msg_browser_crashed 14
  @msg_console_log 15
  @msg_channel_msg 16

  @moduledoc false

  @spec go_to_url(String.t()) :: <<_::8, _::_*8>>
  def go_to_url(url), do: <<@msg_go_to_url, url::binary>>

  @spec run_javascript(String.t()) :: <<_::8, _::_*8>>
  def run_javascript(code), do: <<@msg_run_javascript, code::binary>>

  @spec blank(boolean()) :: <<_::16>>
  def blank(true), do: <<@msg_blank, 1>>
  def blank(false), do: <<@msg_blank, 0>>

  @spec reload() :: <<_::8>>
  def reload(), do: <<@msg_reload>>

  @spec go_back() :: <<_::8>>
  def go_back(), do: <<@msg_go_back>>

  @spec go_forward() :: <<_::8>>
  def go_forward(), do: <<@msg_go_forward>>

  @spec stop_loading() :: <<_::8>>
  def stop_loading(), do: <<@msg_stop_loading>>

  @spec set_zoom(number()) :: <<_::8, _::_*8>>
  def set_zoom(factor) when is_number(factor) do
    str = to_string(factor)
    <<@msg_set_zoom, str::binary>>
  end

  @spec decode(binary()) ::
          :finished_loading_page
          | :started_loading_page
          | :wakeup
          | {:progress, byte()}
          | {:unknown, byte()}
          | {:url_changed, String.t()}
          | {:browser_crashed, atom(), byte()}
          | {:console_log, String.t()}
          | {:channel_message, String.t()}
  def decode(<<@msg_progress, value>>), do: {:progress, value}
  def decode(<<@msg_url_changed, url::binary>>), do: {:url_changed, url}
  def decode(<<@msg_loading_page>>), do: :started_loading_page
  def decode(<<@msg_finished_loading_page>>), do: :finished_loading_page
  def decode(<<@msg_wakeup>>), do: :wakeup

  def decode(<<@msg_browser_crashed, status, exit_code>>) do
    {:browser_crashed, termination_status(status), exit_code}
  end

  def decode(<<@msg_console_log, message::binary>>) do
    {:console_log, message}
  end

  def decode(<<@msg_channel_msg, message::binary>>) do
    {:channel_message, message}
  end

  def decode(<<msg_type, _payload::binary>>), do: {:unknown, msg_type}

  defp termination_status(0), do: :normal
  defp termination_status(1), do: :abnormal
  defp termination_status(2), do: :crashed
  defp termination_status(3), do: :killed
  defp termination_status(n), do: {:unknown, n}
end
