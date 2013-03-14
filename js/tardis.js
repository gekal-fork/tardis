// The following javascript adds accordion-style functionality
// to years and months.
(function($){
  $(function(){
    // The variable "year" takes the argument YYYY from the URL
    // and is passed by Drupal.
    year = Drupal.settings.tardis.year;
    $("#tardis-accordion .tardis-link-year>*>a")
      .attr("href", "javascript:void(0);");
    $("#tardis-accordion .tardis-year-" + year + ">.item-list")
      .addClass("active");
    $("#tardis-accordion *>.item-list:not(.active)").hide();
    $("#tardis-accordion .tardis-link-year")
    .click(function(){
      $(this).siblings().children(".item-list").hide();
      $(this).children(".item-list").addClass("active");
      $(this).children(".item-list.active").show();
    });
  })
})(jQuery);