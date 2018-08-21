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

  @moduledoc false

  @spec go_to_url(String.t()) :: binary()
  def go_to_url(url), do: <<@msg_go_to_url, url::binary>>

  @spec run_javascript(String.t()) :: binary()
  def run_javascript(code), do: <<@msg_run_javascript, code::binary>>

  @spec blank(boolean()) :: binary()
  def blank(true), do: <<@msg_blank, 1>>
  def blank(false), do: <<@msg_blank, 0>>

  @spec reload() :: binary()
  def reload(), do: <<@msg_reload>>

  @spec go_back() :: binary()
  def go_back(), do: <<@msg_go_back>>

  @spec go_forward() :: binary()
  def go_forward(), do: <<@msg_go_forward>>

  @spec stop_loading() :: binary()
  def stop_loading(), do: <<@msg_stop_loading>>

  def decode(<<@msg_progress, value>>), do: {:progress, value}
  def decode(<<@msg_url_changed, url::binary>>), do: {:url_changed, url}
  def decode(<<@msg_loading_page>>), do: :started_loading_page
  def decode(<<@msg_finished_loading_page>>), do: :finished_loading_page
  def decode(<<@msg_wakeup>>), do: :wakeup
  def decode(<<msg_type, _payload::binary>>), do: {:unknown, msg_type}
end
