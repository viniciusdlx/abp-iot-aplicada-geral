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
  const TABLE_HEAD = ["Senha", "Biometria", "RFID"];

  useEffect(() => {
    axios
      .get(process.env.NEXT_PUBLIC_ROUTE_READ || "")
      .then((res: any) => {
        console.log("res -> ", res);
        setData(res?.data?.feeds);
      })
      .catch((err) => {
        console.log(err);
      });
  }, []);

  return (
    <ThemeProvider>
      <main className="flex min-h-screen flex-col items-center justify-between p-12">
        <div className="z-10 max-w-[690px] w-full items-center justify-between font-mono text-sm lg:flex">
          <Card className="w-full px-24 py-4">
            <CardBody>
              <Link href="/cadastro">
                <Button style={{ marginBottom: 18 }}>
                  Cadastro de usuários
                </Button>
              </Link>

              <Card className="h-full w-full">
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
                      data.map(({ field2, field3, field4 }, index) => {
                        const onlyShowsIfAllFull = field2 && field3 && field4;
                        const withoutNulls =
                          field2 != "null" &&
                          field3 != "null" &&
                          field4 != "null";

                        if (onlyShowsIfAllFull && withoutNulls)
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
                            </tr>
                          );
                      })}
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
            </CardBody>
          </Card>
        </div>
      </main>
    </ThemeProvider>
  );
}
