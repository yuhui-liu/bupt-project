/** @type {import('next').NextConfig} */
const nextConfig = {
  images: {
    domains: ['localhost', 'api.good-service.com'],
    formats: ['image/webp', 'image/avif'],
  },
  // Production optimization
  swcMinify: true,
  compiler: {
    removeConsole: process.env.NODE_ENV === 'production',
  },
  // Output standalone for Docker
  output: 'standalone',
};

module.exports = nextConfig;
