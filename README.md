# WebengineKiosk

[![CircleCI](https://circleci.com/gh/nerves-web-kiosk/webengine_kiosk.svg?style=svg)](https://circleci.com/gh/nerves-web-kiosk/webengine_kiosk)
[![Hex version](https://img.shields.io/hexpm/v/webengine_kiosk.svg "Hex version")](https://hex.pm/packages/webengine_kiosk)

Launch and control a fullscreen web browser from Elixir. This is intended for
kiosks running [Nerves](https://nerves-project.org/) but can be used anywhere
you need to show a user a local web-based UI.

Here's an example run:

```elixir
iex> {:ok, kiosk} = WebengineKiosk.start_link(fullscreen: false)
iex> WebengineKiosk.go_to_url(kiosk, "https://elixir-lang.org/")
iex> WebengineKiosk.run_javascript(kiosk, "window.alert('Hello, Elixir!')")
iex> WebengineKiosk.stop(kiosk)
```

It can also be linked into your application's supervision tree:

```elixir
# Example childspecs

   [
      {WebengineKiosk, {[homepage: "https://somewhere.com", background: "black"], name: Display}}
   ]

# Somewhere else in your code

WebengineKiosk.run_javascript(Display, "window.alert('Hello, Elixir!')")
```

## Kiosk options

The kiosk starts fullscreen and goes to a default local web page. To change
this, set one or more options:

* `background_color: color` - specify a background color as #RRGGBB or by name
* `blank_image: path` - specify a path to an image for when the screen is blanked
* `data_dir: path` - specify a writable path for data files
* `debug_keys: boolean` - enable key combinations useful for debugging
* `fullscreen: boolean` - show fullscreen
* `gid: gid` - run the browser with this group id
* `homepage: url` - load this page first. For local files, specify `file:///path/to/index.html`
* `monitor: index` - select the monitor for the web browser (0, 1, etc.)
* `opengl: "gl" | "gles" | "software" | "auto"` - specify the OpenGL backend. This is only a hint.
* `progress: boolean` - show a progress bar when loading pages
* `run_as_root: boolean` - set to true if you really want to run Chromium as root
* `sounds: boolean` - play sounds on clicks
* `uid: uid` - run the browser as this user

See `lib/webengine_kiosk.ex` for some untested options.

## Events

Process can subscribe to receive events from `WebengineKiosk`. Subscribe using `register` function like this from another process:

```elixir
WebengineKiosk.register(Display, self())
```

If the subscribing process is `GenServer` you can handle events like this:

```elixir
def handle_info({:webengine_kiosk, {:channel_message, message}}, state) do
  # Do something with event ...
  {:noreply, state}
end
```

## Sending messages back to elixir

You can send data from Javascript back to elixir using `QtWebChannel`.

First you need to load appropriate javascript library:

Place this somewhere in `<head>`
```html
<script type="text/javascript" src="qrc:///qtwebchannel/qwebchannel.js"></script>
```

Than in your javascript code initialize `QtWebChannel`:

```javascript
new QWebChannel(qt.webChannelTransport, function (channel) {
  var JSobject = channel.objects.elixirJsChannel;
  window.elixirJsChannel = JSobject;
});
```

than anywhere in your javascript code you can call following function to send messages:

```javascript
window.elixirJsChannel.send("Hello world!");
```

The message will appear as `:channel_message` event. Only text can be sent, but if you want so send objects, you can serialize the to JSON and send them as such.

## Installation

`WebengineKiosk` requires [Qt](http://qt.io/) version 5.10 or later (it may
work on earlier versions, but we haven't tested it). It is likely that your
package manager already has a Qt package.

On Debian or Ubuntu:

```sh
sudo apt install qtwebengine5-dev qtmultimedia5-dev qt5-default
```

On OSX:

```sh
brew install qt

# Homebrew doesn't automatically add `qmake` to your path, so run this when
# building or add it to your .bashrc, .zshrc, etc.
export PATH="/usr/local/opt/qt/bin:$PATH"
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
    {:webengine_kiosk, "~> 0.2"}
  ]
end
```

## Permissions

`WebengineKiosk` will refuse to run as the root user. You may specify a name or
number using the `:uid` and `:gid` parameters to `WebengineKiosk.start_link/2`.
If unspecified and running as root, `WebengineKiosk` will try to drop to a
`kiosk` user and `kiosk` group by default. If dropping privileges, then you also
need to ensure that `QtWebEngine` has a writable data directory. Use the
`:data_dir` option do do this.

The next set of permissions to check are platform-specific. Qt and Chromium
directly access videos drivers and input devices. Usually the device files
associated with those have group permissions and adding the user to the
appropriate groups makes everything work. For example, on the Raspberry Pi,
you'll need the kiosk user to be part of the `video` and `input` groups. You may
also need to update the permissions and group ownership on `/dev/vchiq` if
running on Nerves since it doesn't yet do that by default.

## Debugging immediate exits

If the `kiosk` binary exits immediately then something basic is wrong.
Unfortunately, errors often get printed to the terminal and don't end up in
Elixir logs. If this happens to you, try running the `kiosk` binary by itself:

```elixir
iex> path = Application.app_dir(:webengine_kiosk, "priv/kiosk")
"/srv/erlang/lib/webengine_kiosk-0.1.0/priv/kiosk"
iex> System.cmd(path, [])
```
