# Alumni Portal
This application was created by students of the University of Hildesheim, Germany for the module "IT-Studienseminar".
It implements a social network for alumni on top of Drupal 7.x (currently: 7.56).  
The features that needed to be implemented were determined by requirements engineering with a potential "customer" of the software.

## Features
Currently the "Alumni Portal" implements the following features:

* Todo
* Todo
* Todo

## Installation
This drupal application uses the [drupal-composer/drupal-project](https://github.com/drupal-composer/drupal-project) as a baseline, that enables you to manage libraries, modules and themes via [Composer](https://getcomposer.org/).
Therefore, to use this application, you need to:

1. Fullfill all [requirements](https://www.drupal.org/docs/7/system-requirements/overview) for a drupal installation. (Or e.g. install [XAMPP](https://www.apachefriends.org/index.html) for local development.)
2. Install [Composer](https://getcomposer.org/download/).
3. Download or clone/fork this repository.
4. Run `composer install` inside the folder (you should be in the directory that 'composer.json' is in) to install all dependencies and build the drupal application
5. Point your webserver to the 'web' subfolder and make your configuration secure
6. Proceed with the usual [drupal configuration](https://www.drupal.org/docs/7/install) from step 2 on.

## Development
If you want to add to this project, please consider the following:

1. Add all drupal contributed libraries, modules or themes via `composer require drupal/normalized_name`. Thanks to drupal-composer, this will place these packages at the right places for you.
2. Add custom libraries to 'sites/all/libraries', custom modules to 'sites/all/modules/custom' and custom themes to 'sites/all/themes/custom'.
3. Remember editing the .gitignore file if you have a real need to persist other directories with Git that are currently ignored (because they should only have files from drupal core, the composer-managed modules or user-generated content in them).
4. Remember using the 'settings.php' file from your development environment. This file is not persisted because it contains sensitive information.

## License

For now, our work is published under the GNU General Public License, Version 3. If you have convincing arguments for a license change, feel free to write an issue about it and we might follow your advice.