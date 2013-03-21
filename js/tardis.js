/**
 * @file
 * This javascript adds accordion-style functionality to years and months.
 */

(function($, Drupal){
  Drupal.behaviors.tardis = {
    attach: function (context, settings) {
      // The variable "year" takes the argument YYYY from the URL
      // and is passed by Drupal.
      year = settings.tardis.year;
      $("#tardis-accordion .tardis-link-year>*>a", context)
        .attr("href", "javascript:void(0);");
      $("#tardis-accordion .tardis-year-" + year + ">.item-list", context)
        .addClass("active");
      $("#tardis-accordion *>.item-list:not(.active)", context).hide();
      $("#tardis-accordion .tardis-link-year", context).click(function () {
        var $this = $(this);
        $this.siblings().children(".item-list").hide();
        $this.children(".item-list").addClass("active");
        $this.children(".item-list.active").show();
      });
    }
  };
})(jQuery, Drupal);