exception GraphqlError(string);

type state('a) =
  | Init
  | Loading
  | Error(exn)
  | Response('a);

type action('a) =
  | Fetch
  | Failure(exn)
  | Success('a);

let initialState = Init;

let reducer = _ =>
  fun
  | Fetch => Loading
  | Failure(error) => Error(error)
  | Success(data) => Response(data);

let useGraphql = (url, query) => {
  let (state, dispatch) = React.useReducer(reducer, initialState);

  React.useEffect1(
    () => {
      if (state == Loading) {
        ignore @@
        Js.Promise.(
          Fetch.(
            fetchWithInit(
              url,
              RequestInit.make(
                ~method_=Post,
                ~body=
                  Js.Dict.fromList([
                    ("query", Js.Json.string(query##query)),
                    ("variables", query##variables),
                  ])
                  |> Js.Json.object_
                  |> Js.Json.stringify
                  |> BodyInit.make,
                ~headers=
                  HeadersInit.makeWithArray([|
                    ("content-type", "application/json"),
                  |]),
                (),
              ),
            )
            |> then_(response =>
                 if (Response.ok(response)) {
                   Response.json(response)
                   |> then_(data =>
                        switch (Js.Json.decodeObject(data)) {
                        | Some(obj) =>
                          obj->Js.Dict.unsafeGet("data")
                          |>
                          query##parse
                          |> (data => dispatch(Success(data)));
                          resolve();
                        | None =>
                          dispatch(
                            Failure(
                              GraphqlError("Response is not a valid object"),
                            ),
                          );
                          resolve();
                        }
                      );
                 } else {
                   dispatch(
                     Failure(
                       GraphqlError(
                         "Request failed: " ++ Response.statusText(response),
                       ),
                     ),
                   );
                   resolve();
                 }
               )
            |> catch(error => {
                 dispatch(Failure(GraphqlError(Js.String.make(error))));
                 resolve();
               })
          )
        );
      };
      None;
    },
    [|state|],
  );

  (state, () => dispatch(Fetch));
};