"use client";
import React, { useState, useEffect } from "react";
import {
  Button,
  Card,
  CardBody,
  ThemeProvider,
  Typography,
} from "@material-tailwind/react";
import Link from "next/link";
import axios from "axios";

export default function Listagem() {
  const [data, setData] = useState([] as any[]);
  const TABLE_HEAD = [
    "Nome do usuário",
    "Senha",
    "Biometria",
    "RFID",
    "Etapa atual",
  ];

  useEffect(() => {
    axios
      .get(process.env.NEXT_PUBLIC_ROUTE_READ || "")
      .then((res: any) => {
        const length = res?.data?.feeds?.length;

        if (length >= 1) {
          setData([res?.data?.feeds[length - 1]]);
        }
      })
      .catch((err) => {
        console.log(err);
      });
  }, []);

  return (
    <ThemeProvider>
      <main className="flex min-h-screen flex-col items-center justify-between p-12">
        <div className="z-10 max-w-[870px] w-full items-center justify-between font-mono text-sm lg:flex">
          <Card className="w-full xl:px-24 py-4">
            <CardBody>
              <Link href="/cadastro">
                <Button style={{ marginBottom: 18 }}>
                  Cadastro de usuários
                </Button>
              </Link>

              {data.length >= 1 ? (
                <>
                  <Card className="h-full w-full overflow-scroll">
                    <table className="w-full min-w-max table-auto text-left">
                      <thead>
                        <tr>
                          {TABLE_HEAD.map((head) => (
                            <th
                              key={head}
                              className="border-b border-blue-gray-100 bg-blue-gray-50 p-4"
                            >
                              <Typography
                                variant="small"
                                color="blue-gray"
                                className="font-normal leading-none opacity-70"
                              >
                                {head}
                              </Typography>
                            </th>
                          ))}
                        </tr>
                      </thead>
                      <tbody>
                        {data &&
                          data.map(
                            (
                              {
                                field1,
                                field2,
                                field3,
                                field4,
                                field5,
                                field6,
                              },
                              index
                            ) => {
                              const onlyShowsIfAllFull =
                                field2 && field3 && field4;
                              const withoutNulls =
                                field2 != "null" &&
                                field3 != "null" &&
                                field4 != "null";

                              let etapa = "Ainda não desbloqueado";

                              switch (field6) {
                                case "1":
                                  etapa =
                                    field5 === "N"
                                      ? "RFID Validado"
                                      : "RFID Cadastrado";
                                  break;
                                case "2":
                                  etapa =
                                    field5 === "N"
                                      ? "Teclado Validado"
                                      : "Teclado Cadastrado";
                                  break;
                                case "3":
                                  etapa =
                                    field5 === "N"
                                      ? "Biometria Validada"
                                      : "Biometria Cadastrada";
                                  break;
                                default:
                                  break;
                              }

                              return (
                                <tr
                                  key={field2}
                                  className="even:bg-blue-gray-50/50"
                                >
                                  <td className="p-4">
                                    <Typography
                                      variant="small"
                                      color="blue-gray"
                                      className="font-normal"
                                    >
                                      {field1}
                                    </Typography>
                                  </td>
                                  <td className="p-4">
                                    <Typography
                                      variant="small"
                                      color="blue-gray"
                                      className="font-normal"
                                    >
                                      {field2}
                                    </Typography>
                                  </td>
                                  <td className="p-4">
                                    <Typography
                                      variant="small"
                                      color="blue-gray"
                                      className="font-normal"
                                    >
                                      {field3}
                                    </Typography>
                                  </td>
                                  <td className="p-4">
                                    <Typography
                                      variant="small"
                                      color="blue-gray"
                                      className="font-normal"
                                    >
                                      {field4}
                                    </Typography>
                                  </td>
                                  <td className="p-4">
                                    <Typography
                                      variant="small"
                                      color="blue-gray"
                                      className="font-normal"
                                    >
                                      {etapa}
                                    </Typography>
                                  </td>
                                </tr>
                              );
                            }
                          )}
                      </tbody>
                    </table>

                    {/* MOSTRA PADRAO THINGSPEAK */}

                    {/* <iframe
                  width="450"
                  height="260"
                  style={{ border: "1px solid #cccccc" }}
                  src="https://thingspeak.com/channels/2354855/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"
                />
                <iframe
                  width="450"
                  height="260"
                  style={{ border: "1px solid #cccccc" }}
                  src="https://thingspeak.com/channels/2354855/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"
                />
                <iframe
                  width="450"
                  height="260"
                  style={{ border: "1px solid #cccccc" }}
                  src="https://thingspeak.com/channels/2354855/charts/4?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"
                /> */}
                  </Card>
                </>
              ) : (
                <>
                  <div className="flex justify-center items-center text-black text-center">
                    Nenhum usuário cadastrado!
                  </div>
                </>
              )}
            </CardBody>
          </Card>
        </div>
      </main>
    </ThemeProvider>
  );
}
