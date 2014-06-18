require 'rake'
require 'rake/clean'
require 'rbconfig'
require "spec/rake/spectask"

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

RDOC_OPTS = ["--line-numbers", "--inline-source", '--main', 'README']

begin
  gem 'hanna-nouveau'
  RDOC_OPTS.concat(['-f', 'hanna'])
rescue Gem::LoadError
end

rdoc_task_class = begin
  require "rdoc/task"
  RDoc::Task
rescue LoadError
  begin
    require "rake/rdoctask"
    Rake::RDocTask
  rescue LoadError, StandardError
  end
end

if rdoc_task_class
  rdoc_task_class.new do |rdoc|
    rdoc.rdoc_dir = "rdoc"
    rdoc.options += RDOC_OPTS
    rdoc.main = "subset_sum.rb"
    rdoc.title = "subset_sum: Simple Subset Sum Solver with C and pure ruby versions"
    rdoc.rdoc_files.add ["subset_sum.rb", "LICENSE"]
  end
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
  sh %{#{RUBY} spec/c_speedup.rb}
  sh %{rake clean}
  puts "Ruby Version"
  sh %{#{RUBY} spec/c_speedup.rb}
end
