# WebengineKiosk

Launch and control a fullscreen webbrowser from Elixir. This is intended for use

Here's an example run:

```elixir
iex> {:ok, kiosk} = WebengineKiosk.start_link(fullscreen: false)
{:ok, #PID<0.180.0>}
iex> WebengineKiosk.go_to_url("https://yahoo.com/")
:ok
```

## Kiosk options

Doc all of the options here!

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `webengine_kiosk` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:webengine_kiosk, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/webengine_kiosk](https://hexdocs.pm/webengine_kiosk).

