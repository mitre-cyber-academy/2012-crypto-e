# encoding: UTF-8
require 'sinatra'
require 'tempfile'

class Application < Sinatra::Base
  set :app_file, __FILE__
  
  post '/sign' do
    temp = Tempfile.new('userInput')
    out = Tempfile.new('output')
    temp.write(request.env["rack.input"].read)
    temp.close
    content_type 'application/octet-stream'
    system("./signer500 RSA_modulus.txt RSA_d.txt #{temp.path} > #{out.path}")
    send_file(out.path)
  end
  
  post '/verify' do
    temp = Tempfile.new('userVerify')
    out = Tempfile.new('verifyOut')
    temp.write(request.env["rack.input"].read)
    temp.close
    content_type 'application/octet-stream'
    system("./verifier500 RSA_modulus.txt RSA_e.txt #{temp.path} > #{out.path}")
    send_file(out.path)
  end
end