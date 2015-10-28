<?php

/**
 * @file
 * Contains the default style plugin.
 */

/**
 * Style plugin to render a list of years and months with links to content
 * organized chronologically.
 *
 * @ingroup views_style_plugins
 */
class tardisviews_plugin_style_monthlist extends views_plugin_style {
  // Configurable path for the TARDIS links
  function options_form(&$form, &$form_state) {
    $form['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Link path'),
      '#default_value' => (isset($this->options['path'])) ? $this->options['path'] : 'tardis',
      '#description' => t('Path prefix for each TARDIS link, eg. example.com<strong>/tardis/</strong>2015/10'),
    );
    parent::options_form($form, $form_state);
  }

  // Renders a simple view with chronological links to content
  // in the form of an indented list of years and months
  function render($display_id = NULL) {
    $path = (isset($this->options['path'])) ? $this->options['path'] : 'tardis';

    $output = '<ul>';

    $time_pool = array();
    foreach ($this->view->result as $id => $result) {
      $date = new DateTime();
      $created = $date->setTimestamp($result->node_created);
      $created_year = $created->format('Y');
      $created_month = $created->format('m');
      // Todo: theme function here
      $time_pool[$created_year][0] = t("<a href='/$path/@year'>" . "@year" . "</a>", array('@year' => $created_year));
      $time_pool[$created_year][$created_month] = t("<a href='/$path/@year/@month'>" . "@year/@month" . "</a>", array('@year' => $created_year, '@month' => $created_month));
    }

    foreach ($time_pool as $key => $value) {
      foreach ($value as $subkey => $subvalue) {
        if ($subkey == 0) {
          $output .= "<li>$subvalue<ul>";
        }
        else {
          $output .= "<li>$subvalue</li>";
        }
      }
      $output .= "</ul></li>";
    }

    $output .= '<ul>';
    return $output;
  }

}