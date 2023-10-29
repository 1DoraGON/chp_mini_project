# chp_mini_project

Les détails de ce projet sont les suivants : nous avons 3 fichiers texte contenant le stock d'un magasin simple. Le premier fichier texte s'appelle "product.txt" et contient tous les produits disponibles dans ce magasin. Chaque ligne représente un produit, voici un exemple de ligne (produit) :

"id:0, nom:"jus d'orange", prix:500, stock:20"

C'est le contenu du premier fichier. Le deuxième fichier concerne les utilisateurs et s'appelle "users.txt". Chaque ligne du fichier représente un utilisateur, voici un exemple de ligne :

user_id:0, nom:"harry maguire"

C'est le contenu du deuxième fichier. Le troisième fichier s'appelle "sells.txt" et contient les ventes. Chaque ligne de ce fichier représente une vente, voici un exemple de ligne :

sell_id:0, user_id:0, product_id:0, product_count:5

C'est le contenu du troisième fichier. Ce que nous voulons faire, c'est utiliser MPI avec le langage C. Nous écrivons un code qui ouvre les trois fichiers différents en utilisant un traitement parallèle et en utilisant MPI pour envoyer et recevoir des messages. Nous communiquons les différents contenus de ces fichiers afin de pouvoir les fusionner et créer un troisième fichier appelé "user_sell_product" qui contient ce qu'un utilisateur a acheté et le prix total. Voici un exemple de ligne dans ce fichier :

id:0, user_id:0, produits:["jus d'orange"], prix_total:2500

Pour tout complément d'information concernant le code, veuillez me contacter.
