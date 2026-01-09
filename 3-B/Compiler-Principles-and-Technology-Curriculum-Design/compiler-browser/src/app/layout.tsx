import type { Metadata } from "next";
import "@/app/globals.css";

export const metadata: Metadata = {
  title: "VGC - Very Good Compiler",
  description: "Compile your Pascal code to C.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="zh">
      <body className="antialiased">{children}</body>
    </html>
  );
}
