require 'rake'
require 'rake/clean'
require 'rbconfig'
require "spec/rake/spectask"
begin
  require "hanna/rdoctask"
rescue LoadError
  require "rake/rdoctask"
end

CLEAN.include %w"rdoc Makefile subset_sum.o subset_sum.so subset_sum-*.gem"
RUBY=ENV['RUBY'] || File.join(RbConfig::CONFIG['bindir'], RbConfig::CONFIG['ruby_install_name'])
ENV['RUBYLIB'] = ".#{File::PATH_SEPARATOR}#{ENV['RUBYLIB']}"

task :default => [:spec]
task :spec => [:build]
Spec::Rake::SpecTask.new("spec") do |t|
  t.spec_files = %w'spec/subset_sum_spec.rb'
end

task :build do
    sh %{#{RUBY} extconf.rb}
    sh %{make}
end

Rake::RDocTask.new do |rdoc|
  rdoc.rdoc_dir = "rdoc"
  rdoc.options += ["--quiet", "--line-numbers", "--inline-source"]
  rdoc.main = "SubsetSum"
  rdoc.title = "subset_sum: Simple Subset Sum Solver with C and pure ruby versions"
  rdoc.rdoc_files.add ["subset_sum.rb", "LICENSE"]
end

desc "Update docs and upload to rubyforge.org"
task :doc_rforge => [:rdoc] do
  sh %{chmod -R g+w rdoc/*}
  sh %{scp -rp rdoc/* rubyforge.org:/var/www/gforge-projects/subset-sum}
end

desc "Package subset_sum"
task :package do
  sh %{gem build subset_sum.gemspec}
end

desc "Install subset_sum"
task :install => [:package] do
  sh %{sudo gem install subset_sum-*.gem}
end

task :speedup => [:build] do
  puts "C Version"
  sh %{spec/c_speedup.rb}
  sh %{rake clean}
  puts "Ruby Version"
  sh %{spec/c_speedup.rb}
end
