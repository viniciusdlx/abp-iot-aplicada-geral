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

function formatExpires(value: string) {
  return value
    .replace(/[^0-9]/g, "")
    .replace(/^([2-9])$/g, "0$1")
    .replace(/^(1{1})([3-9]{1})$/g, "0$1/$2")
    .replace(/^0{1,}/g, "0")
    .replace(/^([0-1]{1}[0-9]{1})([0-9]{1,2}).*/g, "$1/$2");
}

export default function CheckoutForm() {
  const [loading, setLoading] = useState(false);
  const { register, setValue, handleSubmit } = useForm();

  useEffect(() => {
    fetchCadastro();
  }, []);

  const fetchCadastro = () => {
    axios
      .get(process.env.NEXT_PUBLIC_ROUTE_READ || "")
      .then((res: any) => {
        const length = res?.data?.feeds?.length;
        console.log("res -> ", res?.data?.feeds[length - 1].field1);
        setValue("usuario", res?.data?.feeds[length - 1].field1 == 1);
      })
      .catch((err) => {
        console.log(err);
      });
  };

  const onSubmit = (values: any) => {
    setLoading(true);
    values = {
      api_key: "7GNSQB81HFDQPX5C",
      field1: values.usuario ? 1 : 0,
    };
    console.log("value", values);
    axios
      .post(process.env.NEXT_PUBLIC_ROUTE_WRITE || "", values)
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
                Está em modo de cadastro?
              </Typography>
              <div className="grid place-content-center h-48">
                <Switch
                  {...register("usuario")}
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
