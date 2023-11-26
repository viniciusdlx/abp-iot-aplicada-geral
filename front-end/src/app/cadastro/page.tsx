"use client";
import { ThemeProvider } from "@material-tailwind/react";
import DefaultForm from "../defaultForm/page";

export default function Cadastro() {
  return (
    <ThemeProvider>
      <main className="flex min-h-screen flex-col items-center justify-between p-12">
        <div className="z-10 max-w-5xl w-full items-center justify-between font-mono text-sm lg:flex">
          <DefaultForm />
        </div>
      </main>
    </ThemeProvider>
  );
}
