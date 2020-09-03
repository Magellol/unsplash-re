open Bs_node_fetch;

module Api = {
  let apiKey = Js.Dict.get(Node.Process.process##env, "UNSPLASH_API_KEY");
  let url = "https://api.unsplash.com";
  let version = "v1";

  let request = (url: string, init: RequestInit.t) => {
    fetchWithInit(url, init);
  };
};

module User = {
  type links = {
    self: string,
    html: string,
    photos: string,
    likes: string,
    portfolio: string,
  };

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
  type position = {
    latitude: float,
    longitude: float,
  };

  type t = {
    city: string,
    country: string,
    position,
  };
};

module Exif = {
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
  type t = {title: string};
};

module Photo = {
  type urls = {
    raw: string,
    full: string,
    regular: string,
    small: string,
    thumb: string,
  };

  type links = {
    self: string,
    html: string,
    download: string,
    download_location: string,
  };

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
    description: string,
    exif: Exif.t,
    location: Location.t,
    tags: list(Tag.t),
    urls,
    links,
    user: User.t,
  };

  let get = id =>
    RequestInit.make(~method_=Get, ()) |> Api.request("/photos/" ++ id);
};
