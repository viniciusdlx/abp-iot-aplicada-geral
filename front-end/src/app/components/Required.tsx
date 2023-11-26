import React from "react";

export default function Required({ errors, name }: { errors: any, name: string }) {
  return (
    <>
      {errors[name] && <p className="text-red-500">{errors[name].message}</p>}
    </>
  );
}
