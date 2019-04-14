let url = "https://movie-database-graphql.herokuapp.com/graphql";

// Partial application here wouldn't work
let useGraphql = query => Hook.UseGraphql.useGraphql(url, query);