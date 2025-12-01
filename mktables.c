#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/string.h>
#include <linux/byteorder/generic.h>

/*
The foundation for this code was borrowed from https://blogs.oracle.com/linux/introduction-to-netfilter, 
a basic guide for writing netfilter hooks
*/

// The hex value of the port which needs to be blocked (0x50 = 80 for HTTP packets)
#define PORT 0x50

static struct nf_hook_ops *nf_blocker_ops = NULL;
static struct nf_hook_ops *nf_blocker_out_ops = NULL;

// Handler function called at pre-route hook.
static unsigned int nf_blocker_handler(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    /*
    sk_buff (socket buffer) is the main networking structure used by the Linux kernel to represent a packet.
    Assigning a variable ip_hdr(skb) or tcp_hdr(skb) is basically just a wrapper of the corresponding
    layer/protocol struct around a segment of the socket buffer.
    */
    if(skb==NULL) {
        return NF_ACCEPT;
    }

    struct tcphdr *tcph = tcp_hdr(skb);

    // This is the heart of the tool. If it matches the destination port you want to block, drop the packet.
    if(ntohs(tcph->dest) == PORT) {
        return NF_DROP;
    }

    return NF_ACCEPT;
}

// Register netfilter hook with kernel
static int __init nf_blocker_init(void) {

    /* 
    struct used to hold operations triggered by the netfilter hook: 
    https://elixir.bootlin.com/linux/v6.8/source/include/linux/netfilter.h#L97 
    */
    nf_blocker_ops = (struct nf_hook_ops*)kcalloc(1,  sizeof(struct nf_hook_ops), GFP_KERNEL);


    // This conditional adds the handler to the ops, as well as specifies the hook (NF_INET_PRE_ROUTING) and priority
    if(nf_blocker_ops!=NULL) { 
        nf_blocker_ops->hook = (nf_hookfn*)nf_blocker_handler;
        nf_blocker_ops->hooknum = NF_INET_PRE_ROUTING;
        nf_blocker_ops->pf = NFPROTO_IPV4;
        nf_blocker_ops->priority = NF_IP_PRI_FIRST;

        nf_register_net_hook(&init_net, nf_blocker_ops);
    }

    nf_blocker_out_ops = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);

    if(nf_blocker_out_ops!=NULL) {
        nf_blocker_out_ops->hook = (nf_hookfn*)nf_blocker_handler;
        nf_blocker_out_ops->hooknum = NF_INET_LOCAL_OUT;
        nf_blocker_out_ops->pf = NFPROTO_IPV4;
        nf_blocker_out_ops->priority = NF_IP_PRI_FIRST;

        nf_register_net_hook(&init_net, nf_blocker_out_ops);
    }

    return 0;
}

// Unregister netfilter hook and free the allocated memory
static void __exit nf_blocker_exit(void) {

    if(nf_blocker_ops != NULL) {
        nf_unregister_net_hook(&init_net, nf_blocker_ops);
        kfree(nf_blocker_ops);
    }

    if(nf_blocker_out_ops != NULL) {
        nf_unregister_net_hook(&init_net, nf_blocker_out_ops);
        kfree(nf_blocker_out_ops);
    }
}

module_init(nf_blocker_init);
module_exit(nf_blocker_exit);

MODULE_LICENSE("GPL");
