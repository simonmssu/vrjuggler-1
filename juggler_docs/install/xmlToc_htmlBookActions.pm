package xmlToc_htmlBookActions;

use strict qw(vars);
use vars qw(@ISA @EXPORT);

@ISA = qw(Exporter);

require 5.003;
require Exporter;

##################################################
# How to use this package:
# .i.e:
# use xmlToc;
# use xmlToc_htmlBookActions;
# xmlToc_htmlBookActions::useme();
# xmlToc::load(\$xmldata, "filename.xml");
# xmlToc::traverse(\$htmldata, $xmldata);
# print $htmldata;
##################################################

@EXPORT = qw(useme pushFolder_action popFolder_action pushFont_action
             popFont_action item_action defaults_action treecontrol_action);

my(%book) = ();
my(@dummy) = ();

$book{'title'}    = '';
$book{'chapters'} = \@dummy;     # This seems like a hack...

sub useme () {
    xmlToc::setAction("folder", \&xmlToc_htmlBookActions::pushFolder_action);
    xmlToc::setAction("/folder", \&xmlToc_htmlBookActions::popFolder_action);
    xmlToc::setAction("item", \&xmlToc_htmlBookActions::item_action);
    xmlToc::setAction("font", \&xmlToc_htmlBookActions::pushFont_action);
    xmlToc::setAction("/font", \&xmlToc_htmlBookActions::popFont_action);
    xmlToc::setAction("idefault", \&xmlToc_htmlBookActions::defaults_action);
    xmlToc::setAction("treecontrol",
                      \&xmlToc_htmlBookActions::treecontrol_action);
}

sub pushFolder_action ($$$$$) {
    my $xmlToc_data_out       = shift; # reference to the data
    my $whatAmI               = shift;
    my $myParams              = shift;
    my $any_content_inbetween = shift;
    my $indent_level          = shift;

    $myParams =~ /title[ ]*=[ ]*"(.*?)"/s;
    my $title = "$1";

    outputIndents($xmlToc_data_out, $indent_level);

    if ( $whatAmI =~ /rootfolder/ ) {
        $book{'title'}       = "$title";
        @{$book{'chapters'}} = ();
    }

    if ( $myParams =~ /link\s*=\s*"(.*?)"/si ) {
        push(@{$book{'chapters'}}, "$1");
    }

    outputIndents($xmlToc_data_out, $indent_level);
}

sub popFolder_action ($$$$$) {
    my $xmlToc_data_out       = shift; # reference to the data
    my $whatAmI               = shift;
    my $myParams              = shift;
    my $any_content_inbetween = shift;
    my $indent_level          = shift;

    outputIndents($xmlToc_data_out, $indent_level);

    if ( $whatAmI =~ /rootfolder/ ) {
        # Insert header.
        $$xmlToc_data_out =~ s|</head>|\n${InstallWeb::html_header}\n</head>\n|is;

        # CSS header insert.  If there is </head> then insert before it.
        if ( $$xmlToc_data_out =~ s/<\/head>/\n${InstallWeb::css_header}\n<\/head>\n/is )
        {
#           print "added css (</head>)...";
        }
        # If not, then look for <html> and insert after it with
        # <head>...</head> around it.
        elsif ( $$xmlToc_data_out =~ s/<html>/<html>\n<head>\n${InstallWeb::css_header}\n<\/head>\n/is )
        {
#           print "added css (<html>)...";
        }
        else
        {
#           print "did NOT add css...";
        }

        my $html_install_prefix = $InstallWeb::html_install_prefix;

        # Edit the CSS link path.
        # ASSERT: The path in the css file is relative to the
        #         root directory of the web install
        $$$xmlToc_data_out =~ s/<link(.*?)\"stylesheet\"(.*?)href(.*?)\"/$&$html_install_prefix/gis;

        foreach ( @{$book{'chapters'}} ) {
            print "+";
            printChapter($xmlToc_data_out, "$_");
        }

        # Insert footer.
        $$xmlToc_data_out =~ s/<\/body>/\n${InstallWeb::html_footer}\n<\/body>/is;
    }
}

sub pushFont_action ($$$$$) {
   my $xmlToc_data_out       = shift; # reference to the data
   my $whatAmI               = shift;
   my $myParams              = shift;
   my $any_content_inbetween = shift;
   my $indent_level          = shift;

   outputIndents($xmlToc_data_out, $indent_level);
}

sub popFont_action ($$$$$) {
    my $xmlToc_data_out       = shift; # reference to the data
    my $whatAmI               = shift;
    my $myParams              = shift;
    my $any_content_inbetween = shift;
    my $indent_level          = shift;

    outputIndents($xmlToc_data_out, $indent_level);
}

sub item_action ($$$$$) {
    my $xmlToc_data_out       = shift; # reference to the data
    my $whatAmI               = shift;
    my $myParams              = shift;
    my $any_content_inbetween = shift;
    my $indent_level          = shift;

    $myParams =~ /title[ ]*=[ ]*"(.*?)"/s;
    my $title = "$1";

    my $link = '';

    if ( $myParams =~ /link[ ]*=[ ]*"(.*?)"/s ) {
        $link = "$1";
        push(@{$book{'chapters'}}, "$link");
    }

    if ( $myParams =~ /jit[ ]*=[ ]*"(.*?)"/s ) {
        my $jitfolder = "";
        xmlToc::load(\$jitfolder, $1);
        xmlToc::traverse($xmlToc_data_out, $jitfolder);
    }
}

sub defaults_action ($$$$$) {
}

sub treecontrol_action($$$$$) {
}

# helper func for outputting indentations
# (takes an integer for num of indentations)
sub outputIndents ($$) {
    my $output_ref   = shift; #reference to data
    my $indent_level = shift;
    my $x            = 0;

    while ( $x < $indent_level ) {
        $x = $x + 1;
        $$output_ref .= "   "; # 3 spaces == one tab
    }
}

sub printChapter ($$) {
    my $output_ref   = shift;
    my $chapter_file = shift;

    my $html ='';

    if ( loadHTML(\$html, "$chapter_file", 0) ) {
        $$output_ref .= "$html";
    }
}

sub loadHTML ($$$$) {
    my $data         = shift;
    my $input_file   = shift;
    my $use_feedback = shift;

    my $status = 1;

    if ( open(INPUT, "$input_file") ) {
        my $input;
        while ( <INPUT> ) {
            $input .= $_;
            print "." if $use_feedback;
        }

        close(INPUT) or warn "WARNING: Cannot close $input_file: $!\n";

        # Strip out the <!doctype...> from $input.
        $input =~ s/<!doctype.*?>//is;

        # Remove the opening and closing html tags.
        $input =~ s/<\s*html\s*>(.*?)<\s*\/html\s*>/\1/is;

        # Strip out the <head>...</head> from $input.
        $input =~ s/<\s*head\s*>.*?<\s*\/head\s*>//is;

        # Remove any body tags.
        $input =~ s/<\s*\/?body.*?>//gis;

        filterHTML(\$input);

        # At this point, the HTML is suitable for appending.
        $$data .= "$input";
    }
    else {
        warn "WARNING: Could not open $input_file for reading: $!\n";
        $status = 0;
    }

    return $status;
}

sub filterHTML ($) {
    my $file_contents = shift;

    my $ignore_all_str = 'install-web ignore';
    my $ignore_tags_str = 'install-web no-tags';
    my $ignore_includes_str = 'install-web no-includes';
    my $ignore_css_str = 'install-web no-css';
    my $ignore_header_str = 'install-web no-header';
    my $ignore_footer_str = 'install-web no-footer';
    my $include_footer_str = 'install-web common-footer';

    my $html_comment_begin = $InstallWeb::html_comment_begin;
    my $html_comment_end   = $InstallWeb::html_comment_end;

    ### Check for IGNORE ALL ####
    if ( $$file_contents =~ m/$ignore_all_str/is ) {
        print "ignoring file.\n";
    }
    else {
        ############### search and replace (tags and includes) ################
        #
        if ( $$file_contents =~ /${html_comment_begin}\s*?$ignore_includes_str\s*?${html_comment_end}/is )
        {
            print "[ignoring includes...]";
        }
        else {
            InstallWeb::processIncludesRecursive($file_contents);
        }

        if ( $$file_contents =~ /${html_comment_begin}\s*?$ignore_tags_str\s*?${html_comment_end}/is )
        {
            print "[ignoring tags...]";
        }
        else {
            InstallWeb::replaceTagsRecursive($file_contents, 0);
        }
        #
        ############### end of search and replace (tags and includes) #########
    }
}

1;
