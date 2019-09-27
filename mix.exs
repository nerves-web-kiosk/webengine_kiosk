defmodule WebengineKiosk.MixProject do
  use Mix.Project

  @version "0.2.4"
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
end
