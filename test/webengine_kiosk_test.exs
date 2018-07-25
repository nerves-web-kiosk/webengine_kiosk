defmodule WebengineKioskTest do
  use ExUnit.Case
  doctest WebengineKiosk

  test "greets the world" do
    assert WebengineKiosk.hello() == :world
  end
end
