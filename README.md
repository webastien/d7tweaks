# Seb Drupal 7 tweaks

## About
sebd7tweaks is a Drupal 7 module I wrote about 2 years ago. I planned to use it as a starter module for my future D7 projetcs, but had no time to really maintain / evolve it. So, **I'm not sure it's fully working now**: Some contrib modules will probably changed and the code in sebd7tweaks could be obsolete.

## Objectives
When I make a project with Drupal, I like to adjust options and UI to simplify the UX because I consider my clients should not spend their time to understand all possibilities offered by out-of-the-box Drupal modules (core and contribs), and want to focus only on what is usefull for their needs. Rather than repeat same tasks on new projects, I tried to centralize them in a helper module.

## Concept
1. The module has dependencies in the **.info** file, even if it could work without lots of them: The reason is when I activate it, those frequently used modules will be automaticaly installed.

2. The module offers options organized by module, you can (dis)able by a simple checkbox in the back-office.

3. Those options are displayed only when the associated module is enable in the project. For example, if you install the module [User field privacy](http://drupal.org/project/user_field_privacy), you'll get an option to make field privates by default.

4. This module also provides helper functions, planned to speed up my development.

## Future of this module
I'm affraid it's already dead, because now Drupal 8 is out and more and more used in new projects, so IMHO, refresh this module has a limited interest. I still share it on my github for archive, and let everyone use it as own risks (or grab pieces of its code).
