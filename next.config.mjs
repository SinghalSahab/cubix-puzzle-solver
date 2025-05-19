/** @type {import('next').NextConfig} */
const nextConfig = {
  images: {
    domains: ['www.picgifs.com'],
    unoptimized: true,
  },
  // Only use export for production builds for GitHub Pages
  ...(process.env.NODE_ENV === 'production' && {
    output: 'export',
    // Set the base path to match your GitHub repository name
    basePath: '/cubix',
    assetPrefix: '/cubix/',
  }),
};

export default nextConfig;
