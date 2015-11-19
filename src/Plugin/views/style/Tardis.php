<?php

/**
 * @file
 * Definition of Drupal\tardis\Plugin\views\style\Tardis.
 */

namespace Drupal\tardis\Plugin\views\style;

use Drupal\Core\Datetime\Element\Datetime;
use Drupal\core\form\FormStateInterface;
use Drupal\views\Plugin\views\style\StylePluginBase;

/**
 * Style plugin to render a list of years and months
 * in reverse chronological order linked to content.
 *
 * @ingroup views_style_plugins
 *
 * @ViewsStyle(
 *   id = "tardis",
 *   title = @Translation("TARDIS"),
 *   help = @Translation("Render a list of years and months in reverse chronological order linked to content."),
 *   display_types = { "normal" }
 * )
 *
 *
 */

class Tardis extends StylePluginBase {
  /**
   * Set default options
   */
  protected function defineOptions() {
    $options = parent::defineOptions();
    $options['path'] = array('default' => 'tardis');
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);
    $form['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Link path'),
      '#default_value' => (isset($this->options['path'])) ? $this->options['path'] : 'tardis',
      '#description' => t('Path prefix for each TARDIS link, eg. example.com<strong>/tardis/</strong>2015/10'),
    );
  }

  // Renders a simple view with chronological links to content
  // in the form of an indented list of years and months
  function render($display_id = NULL) {
    $path = (isset($this->options['path'])) ? $this->options['path'] : 'tardis';

    $output = '<ul>';

    $time_pool = array();
    foreach ($this->view->result as $id => $result) {
      $created = $result->node_field_data_created;
      $created_year = date('Y', $created);
      $created_month = date('m', $created);
      $time_pool[$created_year][0] = t("<a href='/$path/@year'>" . "@year" . "</a>", array('@year' => $created_year));
      $time_pool[$created_year][$created_month] = t("<a href='/$path/@year/@month'>" . "@year/@month" . "</a>", array('@year' => $created_year, '@month' => $created_month));
/*
      kint($created_year);
      // Todo: theme function here
*/
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
