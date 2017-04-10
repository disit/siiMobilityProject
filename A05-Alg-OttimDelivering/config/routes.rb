Rails.application.routes.draw do
  # For details on the DSL available within this file, see http://guides.rubyonrails.org/routing.html
  get "api/doc"
  post "api/shortest_path" , to: "api#shortest_path"
  post "api/test/shortest_path" , to: "api#shortest_path_geojson"
end
