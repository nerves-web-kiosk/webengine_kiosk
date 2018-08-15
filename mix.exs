defmodule WebengineKiosk.MixProject do
  use Mix.Project

  def project do
    [
      app: :webengine_kiosk,
      version: "0.1.0",
      elixir: "~> 1.6",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make | Mix.compilers()],
      make_clean: ["clean"],
      deps: deps(),
      docs: [extras: ["README.md"]],
      description: description(),
      package: package()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp description do
    "Display and control web pages on a local fullscreen browser"
  end

  defp package do
    [
      files: [
        "lib",
        "src/main.cpp",
        "src/Blanking.cpp",
        "src/Blanking.h",
        "src/ElixirComs.cpp",
        "src/ElixirComs.h",
        "src/Kiosk*.cpp",
        "src/Kiosk*.h",
        "src/kiosk.pro",
        "src/ui.qrc",
        "src/ui",
        "test",
        "mix.exs",
        "README.md",
        "LICENSE",
        "CHANGELOG.md",
        "Makefile"
      ],
      maintainers: ["Frank Hunleth", "Justin Schneck"],
      licenses: ["Apache-2.0", "LGPL-3.0-only"],
      links: %{"GitHub" => "https://github.com/fhunleth/webengine_kiosk"}
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.4", runtime: false},
      {:ex_doc, "~> 0.18.0", only: :dev}
    ]
  end
end
