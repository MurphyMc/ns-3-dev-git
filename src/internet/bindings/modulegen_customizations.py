import os

from pybindgen import param, retval

def post_register_methods(root_module):
    # Fix Add() and Lookup(), which aren't refcounted.
    # (And are therefore dangerous to use in Python, but dangerous trumps absent.)
    cls = [c for c in root_module.classes if c.full_name == 'ns3::ArpCache'][0]
    cls.add_method('Add', 
                   retval('ns3::ArpCache::Entry *', return_internal_reference=True),
                   [param('ns3::Ipv4Address', 'to')])
    cls.add_method('Lookup',
                   retval('ns3::ArpCache::Entry *', return_internal_reference=False),
                   [param('ns3::Ipv4Address', 'destination')])

    # Allow passing null to MarkWaitReply()
    cls = [c for c in root_module.classes if c.full_name == 'ns3::ArpCache::Entry'][0]
    cls.add_method('MarkWaitReply', 
                   'void', 
                   [param('ns3::Ptr< ns3::Packet >', 'waiting', null_ok=True)])
