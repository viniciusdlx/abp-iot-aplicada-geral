"use client";
import React, { useState, useEffect } from "react";
import {
  Card,
  CardBody,
  Input,
  Button,
  Typography,
  Spinner,
  Switch,
} from "@material-tailwind/react";
import { useForm } from "react-hook-form";
import axios from "axios";
import Link from "next/link";
import Swal from "sweetalert2";

export default function CheckoutForm() {
  const [loading, setLoading] = useState(false);
  const [usuario, setUsuario] = useState(false);
  const { register, setValue, handleSubmit } = useForm();

  useEffect(() => {
    fetchCadastro();
  }, []);

  const fetchCadastro = () => {
    axios
      .get(process.env.NEXT_PUBLIC_ROUTE_READ || "")
      .then((res: any) => {
        const length = res?.data?.feeds?.length;
        setValue("cadastro", res?.data?.feeds[length - 1].field5 == "S");
        setValue("usuario", res?.data?.feeds[length - 1].field1);
        setUsuario(res?.data?.feeds[length - 1].field1);
      })
      .catch((err) => {
        console.log(err);
      });
  };

  const onSubmit = (values: any) => {
    setLoading(true);
    let rep = {
      api_key: "7GNSQB81HFDQPX5C",
      field5: values.cadastro ? "S" : "N",
      field1: values.usuario || usuario,
      field6: 0,
    } as any;

    axios
      .post(process.env.NEXT_PUBLIC_ROUTE_WRITE || "", rep)
      .then((res) => {
        console.log(res);
        if (res?.data == 0) {
          throw "É necessário esperar para inserir novamente!";
        }
      })
      .catch((err) => {
        console.log(err);
        Swal.fire({
          title: "Erro!",
          text: err,
          icon: "error",
          confirmButtonText: "OK",
        });
      })
      .finally(() => {
        setLoading(false);
        fetchCadastro();
      });
  };

  return (
    <>
      {loading ? (
        <Card className="w-full px-24 py-4">
          <Spinner />
        </Card>
      ) : (
        <Card className="w-full px-24 py-4">
          <CardBody>
            <Link href="/listagem">
              <Button style={{ marginBottom: 18 }}>Listagem de usuários</Button>
            </Link>
          </CardBody>

          <form className=" flex flex-col" onSubmit={handleSubmit(onSubmit)}>
            <div>
              <Typography
                variant="h1"
                color="blue-gray"
                className="mb-2 font-medium"
              >
                Olá {usuario}!
              </Typography>
              <Typography
                variant="h1"
                color="blue-gray"
                className="mb-2 font-medium"
              >
                Está em modo de cadastro?
              </Typography>
              <div>
                <Input {...register("usuario")} label="Nome de usuário" />
              </div>
              <div className="grid place-content-center h-48">
                <Switch
                  {...register("cadastro")}
                  color="green"
                  ripple={false}
                  className="h-full w-full checked:bg-[#2ec946]"
                  containerProps={{
                    className: "w-[210px] h-[110px]",
                  }}
                  circleProps={{
                    className:
                      "w-[100px] h-[100px] before:hidden left-1.5 border-none",
                  }}
                />
              </div>
            </div>
            <Button className="my-6" type="submit">
              Finalizar
            </Button>
          </form>
        </Card>
      )}
    </>
  );
}
