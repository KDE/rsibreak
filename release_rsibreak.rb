#!/usr/bin/env ruby
#
# Ruby script for generating amaroK tarball releases from KDE SVN
#
# (c) 2005 Mark Kretschmann <markey@web.de>
# Some parts of this code taken from cvs2dist
# License: GNU General Public License V2


name       = "rsibreak"
egmodule   = "utils"
version    = "0.6.0"
docs       = "no"

svnbase    = "https://toma@svn.kde.org/home/kde"
svnroot    = "#{svnbase}/trunk"
adminroot  = "#{svnbase}/branches/KDE/3.5"

addDocs    = []
addPo      = []

#----------------------------------------------------------------

folder     = name + "-" + version
addPo      = [name] + addPo
addDocs    = [name] + addDocs

# Prevent using unsermake
oldmake = ENV["UNSERMAKE"]
ENV["UNSERMAKE"] = "no"

puts "Fetching #{egmodule}/#{name}..."
# Remove old folder, if exists
`rm -rf #{folder} 2> /dev/null`
`rm -rf folder.tar.bz2 2> /dev/null`

Dir.mkdir( folder )
Dir.chdir( folder )

`svn co -N #{svnroot}/playground/#{egmodule}`
Dir.chdir( egmodule )
`svn up #{name}`
`svn up -N doc`

if ( docs != "no")
    for dg in addDocs
        dg.chomp!
        `svn up doc/#{dg}`
    end
else
    `echo 'DO_NOT_COMPILE="$DO_NOT_COMPILE doc"' >> doc/configure.in.in`
end

`svn co #{adminroot}/kde-common/admin`
puts "done\n"

puts "\n"
puts "Fetching l10n docs for #{egmodule}/#{name}...\n"
puts "\n"

i18nlangs = `svn cat #{svnroot}/l10n/subdirs`
i18nlangsCleaned = []
for lang in i18nlangs
  l = lang.chomp
  if (l != "xx")
    i18nlangsCleaned += [l];
  end
end
i18nlangs = i18nlangsCleaned

Dir.mkdir( "l10n" )
Dir.chdir( "l10n" )

# docs
for lang in i18nlangs
  lang.chomp!

  for dg in addDocs
    dg.chomp!
    `rm -rf #{dg}`
    docdirname = "l10n/#{lang}/docs/playground-#{egmodule}/#{dg}"
    if ( docs != "no")
        `svn co -q #{svnroot}/#{docdirname} > /dev/null 2>&1`
    end
    next unless FileTest.exists?( dg )
    print "Copying #{lang}'s #{dg} documentation over...  "
    `cp -R #{dg}/ ../doc/#{lang}_#{dg}`

    # we don't want KDE_DOCS = AUTO, cause that makes the
    # build system assume that the name of the app is the
    # same as the name of the dir the Makefile.am is in.
    # Instead, we explicitly pass the name..
    makefile = File.new( "../doc/#{lang}_#{dg}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
    makefile << "KDE_LANG = #{lang}\n"
    makefile << "KDE_DOCS = #{dg}\n"
    makefile.close()

    puts( "done.\n" )
  end
end

puts "\n"
puts "Fetching l10n po for #{egmodule}/#{name}...\n"
puts "\n"

Dir.chdir( ".." ) # in egmodule now

$subdirs = false
Dir.mkdir( "po" )

for lang in i18nlangs
  lang.chomp!
  dest = "po/#{lang}"

  for dg in addPo
    dg.chomp!
    pofilename = "l10n/#{lang}/messages/playground-#{egmodule}/#{dg}.po"
    `svn cat #{svnroot}/#{pofilename} 2> /dev/null | tee l10n/#{dg}.po`
    next if FileTest.size( "l10n/#{dg}.po" ) == 0

    if !FileTest.exist?( dest )
      Dir.mkdir( dest )
    end

    print "Copying #{lang}'s #{dg}.po over ..  "
    `mv l10n/#{dg}.po #{dest}`
    puts( "done.\n" )

    makefile = File.new( "#{dest}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
    makefile << "KDE_LANG = #{lang}\n"
    makefile << "SUBDIRS  = $(AUTODIRS)\n"
    makefile << "POFILES  = AUTO\n"
    makefile.close()

    $subdirs = true
  end
end

if $subdirs
  makefile = File.new( "po/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
  makefile << "SUBDIRS = $(AUTODIRS)\n"
  makefile.close()
else
  `rm -Rf po`
end

`rm -rf l10n`
puts "\n"

# Remove SVN data folder
`find -name ".svn" | xargs rm -rf`

`/bin/mv * ..`
Dir.chdir( ".." ) # name-version
`rmdir #{egmodule}`

# Move some important files to the root folder
Dir.chdir( "#{name}" )
`/bin/mv -f src/#{name}.lsm ..`
`/bin/mv -f AUTHORS ..`
`/bin/mv -f COPYING ..`
`/bin/mv -f INSTALL ..`
`/bin/mv -f ChangeLog ..`
`/bin/mv -f NEWS ..`
#`/bin/mv -f README ..`
`/bin/mv -f TODO ..`
#`/bin/mv -f HACKING ..`
`/bin/rm release_rsibreak.rb makechangelog ../INDEX ../README icons/artwork-v1.tar.gz icons/artwork-v2.tar.gz`
Dir.chdir( ".." )


# Generate makefiles
`find | xargs touch`

puts "\n"
puts "Generating Makefiles..  "

# Remove that ugly warning which is added just because we sit in playground
`echo "AUTOMAKE_OPTIONS = foreign" > Makefile.am.in`
`echo "DISTCLEANFILES = inst-apps" >> Makefile.am.in`
`echo "include admin/deps.am" >> Makefile.am.in`
`echo "include admin/Doxyfile.am" >> Makefile.am.in`

`make -f Makefile.cvs`
puts "done.\n"

`rm -rf autom4te.cache`
`rm stamp-h.in`



puts "\n"
puts "Compressing..  "
Dir.chdir( ".." ) # root folder
`tar -jcf #{folder}.tar.bz2 #{folder}`
`rm -rf #{folder}`
puts "done.\n"


ENV["UNSERMAKE"] = oldmake
