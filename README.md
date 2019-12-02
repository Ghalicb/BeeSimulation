## Présentation du programme

Notre programme permet de simuler l'évolution d'un environnement composé d’un terrain, de fleurs, de ruches et d’abeilles.  
  
Tout d’abord, un environnement est un terrain composé de trois types d’éléments:
- L’herbe, où l’on peut poser divers objets. Les abeilles peuvent notamment voler sur cette surface.
- L’eau, où l’on ne peut poser d’objets, mais les abeilles peuvent quand même survoler cette surface. (A noter que la présence d’eau joue un rôle important sur le facteur d’humidité et donc sur le développement des fleurs)
- La roche, entièrement opaque au mouvement ou à la « pose ».
	
Le programme modélise aussi des fleurs qui peuvent être posé ou généré aléatoirement.

Enfin, des ruches contenant deux types d’abeilles sont intégrés à l’environnement :
- Les ScoutBees (éclaireuse), petite et rapide, dévouées toute leur vie à la recherche de fleurs.
- Les WorkerBees (butineuse), costaud et plus lent, allant butiner les fleurs communiquées par les éclaireuses.   
	
A noter que le programme permet d’observer quelques données sur la simulation dans un mode « Debug » dont on reparlera plus tard.  

Et pour plus de convivialité, pensez à augmenter le son ou à mettre un casque … ;)

## Compilation du programme

Pour compiler le programme, il est nécessaire de lancer la commande (dans la partie5) :  
	scons application-run  
depuis le répertoire contenant Sconstruct.
A noter que l’on peut changer la taille du terrain en ajoutant l’option ’—-cfg=appX.json’ (où X est un chiffre entre 1 et 4, où le fichier 3 est celui recommandé).

## Utilisation du programme
	
Mise en place des différents éléments de l’environnement :  
1. Fleur : Placer la souris dans l’environnement et appuyer sur la touche « f ».
2. Ruche : Placer la souris dans l’environnement et appuyer sur la touche « h ».  

Gestion de l’environnement :  
- LoadFromFile : Pour charger un fichier, appuyer sur « i ».
- SaveToFile : Pour sauvegarder un fichier qui vous a plus, appuyer sur « o ».
- Reset : appuyer sur "r" pour recommencer la simulation (cette option peut prendre du temps, une musique d’attente vous est proposé pour passer le temps).
- Update : Appuyer sur « espace » pour activer/désactiver la simulation (à noter que la simulation est initialement désactivé).

Options d’affichage :  
- Mode Debug : en appuyant sur la touche « d », vous activez le mode debug qui vous donnera des informations sur divers éléments de la simulation.
	- Mode Humidité : en appuyant sur la touche « w », vous pourrez voir les sous-terrains de la map (avec notamment les zones d’humidités).
	

	D. Suggestion pour plus d’amusement ;)

	- La simulation a tourné en une sorte de jeu où l’utilisateur peut mettre en compétition plusieurs ruches et voir laquelle atteindra un certain seuil de Nectar.
Pensez une fois, à mettre le mode debug pour voir des animations intéressantes lorsqu’une ruche a gagné !

	- Si vous lancez la simulation sur app.json, vous verrez un changement de texture (un monde de feu). Vous comprendrez très vite pourquoi, lorsque vos ruches mourront par manque de nectar … :S

	- Si vous avez le temps et en voulez plus, vous pouvez aller à la partie2 du projet et lancer le test WorldTest-run. Si vous le faites, vous pourrez découvrir différents mondes grâce aux touches « e », « r » et « t »
