exception GraphqlError(string);

type state('a) =
  | Init
  | Loading
  | Error(exn)
  | Response('a);

let useGraphql:
  (
    string,
    {
      ..
      "parse": Js.Json.t => 'a,
      "query": string,
      "variables": Js.Json.t,
    }
  ) =>
  (state('a), unit => unit);