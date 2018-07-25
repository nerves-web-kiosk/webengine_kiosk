defmodule WebengineKiosk.MixProject do
  use Mix.Project

  def project do
    [
      app: :webengine_kiosk,
      version: "0.1.0",
      description: description(),
      package: package(),
      compilers: [:elixir_make | Mix.compilers()],
      make_clean: ["clean"],
      elixir: "~> 1.6",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp description do
    "Qt WebEngine-based kiosk"
  end

  defp package do
    %{
      maintainers: ["Frank Hunleth", "Justin Schneck"],
      licenses: ["TBD"],
      links: %{"GitHub" => "https://github.com/fhunleth/webengine_kiosk"}
    }
  end

  defp deps do
    [
      {:muontrap, "~> 0.2"},
      {:elixir_make, "~> 0.4", runtime: false},
      {:ex_doc, "~> 0.11", only: :dev}
    ]
  end
end
