# WebengineKiosk

[![CircleCI](https://circleci.com/gh/fhunleth/webengine_kiosk.svg?style=svg)](https://circleci.com/gh/fhunleth/webengine_kiosk)
[![Hex version](https://img.shields.io/hexpm/v/webengine_kiosk.svg "Hex version")](https://hex.pm/packages/webengine_kiosk)

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

`WebengineKiosk` requires [Qt](http://qt.io/). It is likely that your package
manager already has a Qt package.

On Debian or Ubuntu:

```sh
sudo apt install qtwebengine5-dev qtmultimedia5-dev qt5-default
```

On OSX:

```sh
brew install qt
```

If you are installing Qt manually, then the first time that you run `mix`,
you'll need to point to the installation location. If you don't, you'll either
get an error that `qmake` isn't found or you'll being using your system's
version of Qt. Here's an example commandline:

```sh
QMAKE=~/Qt/5.11.1/gcc_64/bin/qmake mix compile
```

Finally, if you're using Nerves, you'll need a Nerves system that includes Qt.
Take a look at
[kiosk_system_rpi3](https://github.com/LeToteTeam/kiosk_system_rpi3) for an
example.

Once you've done all that, go ahead and add `webengine_kiosk` to your `mix.exs`
dependencies like normal:

```elixir
def deps do
  [
    {:webengine_kiosk, "~> 0.1.0"}
  ]
end
```

Documentation can be found at
[https://hexdocs.pm/webengine_kiosk](https://hexdocs.pm/webengine_kiosk).

