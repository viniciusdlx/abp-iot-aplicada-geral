'use client'
import React, { useState } from "react";
import {
  Card,
  Input,
  Button,
  Typography,
  Spinner,
} from "@material-tailwind/react";
import { useForm } from "react-hook-form";
import axios from "axios";
import Swal from "sweetalert2";
import Required from "./components/Required";

type FormInputs = {
  password: string,
  email: string
}

export default function CheckoutForm() {
  const [loading, setLoading] = useState(false);

  const { register, handleSubmit, formState: { errors }, } = useForm<FormInputs>();

  const onSubmit = (values: any) => {
    if (values.email === 'artur-tiscoski@hotmail.com' && values.password === '123123') {
      window.location.href = '/listagem';
    } else {
      Swal.fire({
        title: 'Erro!',
        text: 'Senha e/ou login errado(s)!',
        icon: 'error',
        confirmButtonText: 'OK'
      })
    }
  };

  return (
    <main className="flex min-h-screen flex-col items-center justify-between p-12">
      <div className="z-10 max-w-sm w-full items-center justify-between font-mono text-sm lg:flex">
        {
          loading
            ?
            <Card className="w-full px-24 py-4">
              <Spinner />
            </Card>
            :
            <Card className="w-full px-8 pb-4 pt-2">
              <div className="text-lg text-black mt-3">Login</div>
              <form className="mt-6 flex flex-col" onSubmit={handleSubmit(onSubmit)}>
                <div className="my-3">
                  <Typography
                    variant="small"
                    color="blue-gray"
                    className="mb-2 font-medium"
                  >
                    E-mail
                  </Typography>
                  <Input
                    {...register("email", { required: 'E-mail é obrigatório' })}
                    type="email"
                    placeholder="name@mail.com"
                    className=" !border-t-blue-gray-200 focus:!border-t-gray-900"
                    labelProps={{
                      className: "before:content-none after:content-none",
                    }}
                  />
                  {Required({ errors, name: 'email' })}
                </div>
                <div>
                  <Typography
                    variant="small"
                    color="blue-gray"
                    className="mb-2 font-medium "
                  >
                    Senha numérica
                  </Typography>

                  <Input
                    {...register("password", { required: 'Senha é obrigatório' })}
                    maxLength={6}
                    type="password"
                    placeholder="000000"
                    className=" !border-t-blue-gray-200 focus:!border-t-gray-900"
                    labelProps={{
                      className: "before:content-none after:content-none",
                    }}
                  />
                  {Required({ errors, name: 'password' })}
                </div>
                <div className="my-4 flex justify-end">
                  <Button type="submit">
                    Logar
                  </Button>
                </div>
              </form>
            </Card>
        }
      </div>
    </main>
  );
}
