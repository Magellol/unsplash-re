open Bs_node_fetch;

module ApiResponse = {
  type t('a) =
    | Success('a)
    | Error(array(string));

  [@decco.decode]
  type errorResponse = {errors: array(string)};

  [@bs.scope "JSON"] [@bs.val] external parse: string => Js.Json.t = "parse";
};

module Api = {
  // TODO: Fix unsafe
  let apiKey =
    Js.Dict.unsafeGet(Node.Process.process##env, "UNSPLASH_API_KEY");

  let baseUrl = "https://api.unsplash.com";
  let version = "v1";

  let handleResponse = (decoder, response) => {
    Js.Promise.(
      Response.text(response)
      |> then_(v => ApiResponse.parse(v) |> resolve)
      |> then_(value => {
           let json =
             Response.ok(response)
               ? switch (decoder(value)) {
                 | Ok(v) => ApiResponse.Success(v)
                 | Error((e: Decco.decodeError)) =>
                   ApiResponse.Error([|e.path ++ " is " ++ e.message|])
                 }
               : (
                 // Making sure the error response from the API is what we think it is
                 switch (ApiResponse.errorResponse_decode(value)) {
                 | Ok(v) => ApiResponse.Error(v.errors)
                 | Error(_error) => ApiResponse.Error([|"Unexpected error"|])
                 }
               );

           resolve(json);
         })
      |> catch(_ => ApiResponse.Error([|"Unexpected response"|]) |> resolve)
    );
  };

  let request = (~url, ~method, ~decoder) => {
    Js.Promise.(
      fetchWithInit(
        baseUrl ++ url,
        RequestInit.make(
          ~method_=method,
          ~headers=
            HeadersInit.make({
              "Content-Type": "application/json",
              "Authorization": "Client-ID " ++ apiKey,
            }),
          (),
        ),
      )
      |> then_(handleResponse(decoder))
      // TODO: Use error message instead?
      |> catch(_ => ApiResponse.Error([|"Unknown error"|]) |> resolve)
    );
  };
};

module User = {
  [@decco.decode]
  type links = {
    self: string,
    html: string,
    photos: string,
    likes: string,
    portfolio: string,
  };

  [@decco.decode]
  type t = {
    id: string,
    updated_at: string,
    username: string,
    name: string,
    portfolio_url: string,
    bio: string,
    location: string,
    total_likes: int,
    total_photos: int,
    total_collections: int,
    links,
  };
};
module Location = {
  [@decco.decode]
  type position = {
    latitude: float,
    longitude: float,
  };

  [@decco.decode]
  type t = {
    city: string,
    country: string,
    position,
  };
};

module Exif = {
  [@decco.decode]
  type t = {
    make: string,
    model: string,
    exposure_time: string,
    aperture: string,
    focal_length: string,
    iso: float,
  };
};

module Tag = {
  [@decco.decode]
  type t = {title: string};
};

module Photo = {
  [@decco.decode]
  type urls = {
    raw: string,
    full: string,
    regular: string,
    small: string,
    thumb: string,
  };

  [@decco.decode]
  type links = {
    self: string,
    html: string,
    download: string,
    download_location: string,
  };

  [@decco.decode]
  type t = {
    id: string,
    created_at: string,
    updated_at: string,
    width: int,
    height: int,
    color: string,
    downloads: int,
    likes: int,
    liked_by_user: bool,
    description: option(string),
    exif: Exif.t,
    location: Location.t,
    tags: list(Tag.t),
    urls,
    links,
    user: User.t,
  };

  let get = id =>
    Api.request(~url="/photos/" ++ id, ~method=Get, ~decoder=t_decode);
};

Photo.get("T41xA8AB81M")
|> Js.Promise.then_(v => {
     Js.log(v);
     Js.Promise.resolve();
   });
