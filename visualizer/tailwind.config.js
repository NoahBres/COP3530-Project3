const colors = require("tailwindcss/colors");

module.exports = {
  content: ["./index.html", "./src/**/*.{vue,js,ts,jsx,tsx}"],

  theme: {
    extend: {
      colors: {
        slate: colors.slate,
        emerald: colors.emerald,
      },
    },
  },
  plugins: [],
};
