export const toast = ({ title, description, variant = "default" }) => {
  console.log(`[${variant.toUpperCase()}] ${title}: ${description}`);
};
