open BsAbstract.Functions;
open BsAbstract.Function.Infix;
open BsAbstract.Option.Infix;

module QueryMovies = [%graphql
  {|
    query Movie($query: String!) {
      movies(query: $query) {
        title
      }
    }
  |}
];

[@react.component]
let make = () => {
  open Graphql;
  let (value, setValue) = React.useState(const(""));
  let (searchState, search) =
    useGraphql(QueryMovies.make(~query=value, ()));

  <div>
    <div>
      <form onSubmit=ReactEvent.Form.preventDefault>
        <input
          onChange={
            ReactEvent.Form.currentTarget
            >. (target => target##value)
            >. (const >. setValue)
          }
          value
        />
        <button
          disabled={searchState == Loading}
          onClick={ignore >. search}
          type_="submit">
          "Search"->React.string
        </button>
      </form>
    </div>
    <div>
      {switch (searchState) {
       | Init => "Type some movie title and return to start!"->React.string
       | Loading => "loading"->React.string
       | Error(_) => "An error occured"->React.string
       | Response(data) =>
         ([||] |? data##movies)
         ->Belt.Array.mapWithIndex((index, movie) =>
             <div key={index->string_of_int}>
               {("" |? (movie >>= (m => m##title)))->React.string}
             </div>
           )
         ->React.array
       }}
    </div>
  </div>;
};