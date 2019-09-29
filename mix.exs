defmodule WebengineKiosk.MixProject do
  use Mix.Project

  @version "0.2.5"
  @source_url "https://github.com/fhunleth/webengine_kiosk"

  def project do
    [
      app: :webengine_kiosk,
      version: @version,
      elixir: "~> 1.6",
      description: description(),
      package: package(),
      source_url: @source_url,
      compilers: [:elixir_make | Mix.compilers()],
      make_targets: ["all"],
      make_clean: ["clean"],
      make_error_message: make_help(),
      docs: docs(),
      dialyzer: [
        flags: [:unmatched_returns, :error_handling, :race_conditions, :underspecs]
      ],
      deps: deps()
    ]
  end

  def application do
    [
      extra_applications: [:logger],
      mod: {WebengineKiosk.Application, []}
    ]
  end

  defp description do
    "Display and control web pages on a local fullscreen browser"
  end

  defp package do
    [
      files: [
        "lib",
        "src",
        "assets",
        "test",
        "mix.exs",
        "README.md",
        "LICENSE",
        "CHANGELOG.md",
        "Makefile"
      ],
      licenses: ["Apache-2.0", "LGPL-3.0-only"],
      links: %{"GitHub" => @source_url}
    ]
  end

  defp deps do
    [
      {:system_registry, "~> 0.8", optional: true},
      {:elixir_make, "~> 0.6", runtime: false},
      {:ex_doc, "~> 0.19", only: [:dev, :test], runtime: false},
      {:dialyxir, "~> 1.0.0-rc.6", only: :dev, runtime: false}
    ]
  end

  defp docs do
    [
      extras: ["README.md"],
      main: "readme",
      source_ref: "v#{@version}",
      source_url: @source_url
    ]
  end

  defp using_nerves? do
    System.get_env("CROSSCOMPILE") != nil
  end

  defp make_help do
    """

    Please look above this message for the compiler error before filing an
    issue. The first error after the text `==> webengine_kiosk` is usually the
    most helpful.

    The webengine_kiosk library requires the Qt framework to build.

    """ <> make_help_os(:os.type(), using_nerves?())
  end

  defp make_help_os({:unix, :darwin}, _nerves) do
    """
    To install Qt using Homebrew, run `brew install qt`. Homebrew doesn't add
    qt to your path, so you'll need to add it or set the QMAKE environment
    variable. For example, `export QMAKE=/usr/local/opt/qt/bin/qmake`.
    """
  end

  defp make_help_os({:unix, _}, true) do
    """
    Please install Qt using your system's package manager or via source. Since
    this is a Nerves-based project, only `qmake` is needed. For Ubuntu, this
    looks like:

    sudo apt install qt5-qmake

    Another option is to set the `QMAKE` environment variable to the path to
    the qmake binary:

    QMAKE=~/Qt/5.11.1/gcc_64/bin/qmake
    """
  end

  defp make_help_os({:unix, _}, false) do
    """
    Please install Qt using your system's package manager or via source. Be
    sure to install the development headers and libraries for Qt Webengine.
    For Ubuntu, this looks like:

    sudo apt install qt5-default qtwebengine5-dev qtmultimedia5-dev
    """
  end

  defp make_help_os({other, _}, _nerves) do
    """
    I'm not familiar with installing Qt5 on #{inspect(other)}. Please install
    Qt5 using your platform's package manager or from source and consider
    making an issue or a  PR to #{@source_url} to benefit other users.

    If you're getting an error that `qmake` isn't found, try setting the
    `QMAKE` environment variable to the path to `qmake`.
    """
  end
end
