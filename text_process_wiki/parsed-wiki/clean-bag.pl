#cleans wikipedia markup pages, returning (for each article given):
#  title\tbag of words\n
#Written by: Michael Agun
#Last Updated: 2/21/2014
my $doc='';
my $skip=0;
my $inpage=0;
while(<>) {
    if (/<page>/) {
        $skip=0;
        $inpage=1;
    } elsif (/<\/page>/) {
        $inpage=0;
$_ = $doc;
$doc = '';

#strip everything outside of the text area (but keep the title)
s/^.*?<title>(.*?)<\/title>.*?<text[^>]*>//;
my $title=$1;

s/<\/text>.*//;

#convert any tabs to spaces, and repeated tabs to single spaces
y/\t/ /s;
##technically harvnb renders as author names and a year, but since it isn't normal text I think we can drop it
#s/\{\{[Ci]ite [^{}]*\}\}/ /g;
#s/\{\{harvnb[^{}]*\}\}/ /g;
##remove special items
#s/\{\{US +Patent[^{}]*\}\}/ /gi;
#s/\{\{Citation[ ]\+needed[^{}]*\}\}/ /gi;

##we probably need a more advanced mechanism for nested quotes
s/\{\{[cb]?quote[ ]*\|([^{}]*)\}\}/\1/g;


#clear any remaining {{}} tags (one layer at a time to not kill text between tags)
#we need to handle quotes at each layer in case there was a quote with nested tags
while(s/\{\{[^{}]*\}\}/ /g) {s/\{\{[cb]?quote[ ]*\|([^{}]*)\}\}/\1/g;}
#finally we clear any unmatched {{ and }}
s/\{\{|\}\}/ /g;

#s/'''''(.*?)'''''/\1/g;
s/'''(.*?)'''/\1/g;
s/''(.*?)''/\1/g;

##remove links #TODO: needs to be fixed/improved
#s/\[\[[^][]*\|([^][|]*)\]\]/\1/g;
#s/\[\[([^][]*)\]\]/\1/g;
##remove links (2nd pass)
#s/\[\[[^][]*\|([^][|]*)\]\]/\1/g;
#s/\[\[([^][]*)\]\]/\1/g;

#eliminate links, does it one layer at a time
while(s/\[\[
    (?:([^][|]*)\|)*
    (?<s>[^][]*)
\]\]/$+{s}/x) {}
#recursion based approach
#while(s/\[\[
#        (?:(?:[^][|]*(?R)?[^][|]*)\|)*
#        (?<s>[^][|]*(?R)?[^][|]*)
#    \]\]/$+{s}/gx) {}


# process escapes
s/&amp;/\&/g;
s/&nbsp;/ /g;
s/&lt;/</g;
s/&gt;/>/g;
s/&quot;/"/g;

#clear xml comments.
#uses a non-greedy search so we don't go through adjacent comments
s/<!--.*?-->//g;

s:<br[ ]*[/]?>:\n:g;

#clear urls
s/\[http[^]\n]*\]/ /g;

#clear any singelton xml tags
s/<.*?\/>/ /g;

#clear any reference xml tags
s:<ref.*?>.*?</ref>: :g;

#now just drop any other xml tags, but we keep the text inside the xml tag
s/<[^>\n]*>/ /g;

##process tags
#s:<[/]\?\(big\|small\|poem\|pre\|nowiki\|ins\|u\|s\|del\|ref\)\( [^>]*\)\?>::g;

#now convert to bag of words
y/\n\t/ /s;
$_ = lc;
y/A-Za-z0-9/ /sc;
s/^[ ]+|[ ]+$//g;

print "$title\t";
print;
print "\n";
$_ = '';
    } elsif (!$skip && /^[ \t]*$/) {
        #do nothing for blank lines
    } elsif ($inpage) {
        s/\n$/ /;
        if(/^[ \t]*=.*=[ \t]*$/) {
            #trim any whitespace and =s from the ends
            s/^[ \t]*=+[ \t]*//;
            s/[ \t]*=+[ \t]*$//;
            #check if this is a section to skip
            if (/^(see also|references|external links|gallery|publications|further reading|notes|books|selected works|awards and honors|awards|honors)$/i) {
                $skip = 1;
            } else {
                $skip = 0;
                $_ = " $_ ";
            }
        }

        #only print lines from non-skipped sections
        if (!$skip) {
            $doc .= "$_";
        }
    }
}
