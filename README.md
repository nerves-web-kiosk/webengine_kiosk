# WebengineKiosk

Launch and control a fullscreen web browser from Elixir. This is intended for
kiosks running [Nerves](https://nerves-project.org/) but can be used anywhere
you need to show a user a local web-based UI.

Here's an example run:

```elixir
iex> {:ok, kiosk} = WebengineKiosk.start_link(fullscreen: false)
iex> WebengineKiosk.go_to_url(kiosk, "https://elixir-lang.org/")
iex> WebengineKiosk.stop(kiosk)
```

It can also be linked into your application's supervision tree:

```elixir
# Example childspecs

   [
      {WebengineKiosk, {[homepage: "https://somewhere.com", background: "black"], name: MyKiosk}}
   ]

# Somewhere else in your code

WebengineKiosk.run_javascript(Display, "window.alert('Hello, Elixir!')")
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
