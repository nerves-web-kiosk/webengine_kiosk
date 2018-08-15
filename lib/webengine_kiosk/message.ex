defmodule WebengineKiosk.Message do
  @msg_go_to_url 1
  @msg_run_javascript 2
  @msg_loading_page 3
  @msg_progress 4
  @msg_finished_loading_page 5
  @msg_url_changed 6
  @msg_blank 7
  @msg_wakeup 8

  @moduledoc false

  @spec go_to_url(String.t()) :: binary()
  def go_to_url(url), do: <<@msg_go_to_url, url::binary>>

  @spec run_javascript(String.t()) :: binary()
  def run_javascript(code), do: <<@msg_run_javascript, code::binary>>

  @spec blank(boolean()) :: <<_::16>>
  def blank(true), do: <<@msg_blank, 1>>
  def blank(false), do: <<@msg_blank, 0>>

  def decode(<<@msg_progress, value>>), do: {:progress, value}
  def decode(<<@msg_url_changed, url::binary>>), do: {:url_changed, url}
  def decode(<<@msg_loading_page>>), do: :started_loading_page
  def decode(<<@msg_finished_loading_page>>), do: :finished_loading_page
  def decode(<<@msg_wakeup>>), do: :wakeup
  def decode(<<msg_type, _payload::binary>>), do: {:unknown, msg_type}
end
